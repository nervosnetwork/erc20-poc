CC := riscv32-unknown-elf-gcc
CFLAGS := -Ideps/secp256k1/include -Ic/schema -Ideps/flatcc/include -DCKB_IMPL_LIBC -O3
SECP256K1_LIB := deps/secp256k1/.libs/libsecp256k1.a
VERIFY_BIN := build/verify
VALIDATE_BIN := build/validate
FLATCC := deps/flatcc/bin/flatcc

all: $(VERIFY_BIN) $(VALIDATE_BIN) cargo

$(VALIDATE_BIN): c/validate.c c/erc20.h c/ckb.h c/schema/erc20_reader.h
	$(CC) $(CFLAGS) -o $@ $<

c/schema/erc20_reader.h: c/erc20.fbs $(FLATCC)
	mkdir -p ./c/schema
	$(FLATCC) -c --reader -o ./c/schema ./c/erc20.fbs

$(VERIFY_BIN): c/verify.c $(SECP256K1_LIB)
	$(CC) $(CFLAGS) -o $@ $^

$(FLATCC):
	cd deps/flatcc && scripts/build.sh

$(SECP256K1_LIB):
	cd deps/secp256k1 && \
		./autogen.sh && \
		CC=riscv32-unknown-elf-gcc LD=riscv32-unknown-elf-gcc ./configure --with-bignum=no --enable-ecmult-static-precomputation --enable-endomorphism --host=riscv32-elf && \
		make

cargo:
	cargo build

clean:
	cd deps/secp256k1 && make clean
	cd deps/flatcc && ./scripts/cleanall.sh
	rm -rf $(VERIFY_BIN) $(VALIDATE_BIN) c/schema

.PHONY: all clean cargo
