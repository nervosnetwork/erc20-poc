CC := riscv64-unknown-elf-gcc
CFLAGS := -Ideps/secp256k1/src -Ideps/secp256k1 -Ic/schema -Ideps/flatcc/include -O3
LDFLAGS := -Wl,-static -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s
SECP256K1_LIB := deps/secp256k1/src/ecmult_static_pre_context.h
ALWAYS_SUCCESS_BIN := build/always_success
VERIFY_BIN := build/verify
VALIDATE_BIN := build/validate
SPIKE_VALIDATE_BIN := build/validate_spike
FLATCC := deps/flatcc/bin/flatcc

all: $(ALWAYS_SUCCESS_BIN) $(VERIFY_BIN) $(VALIDATE_BIN) $(SPIKE_VALIDATE_BIN) cargo

$(ALWAYS_SUCCESS_BIN): c/always_success.S
	$(CC) $(CFLAGS) $(LDFLAGS) -nostdlib -nostartfiles -o $@ $<

$(VALIDATE_BIN): c/validate.c c/erc20.h c/ckb.h c/ckb_impl_syscall.h c/schema/erc20_reader.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< -DCKB_IMPL_SYSCALL

$(SPIKE_VALIDATE_BIN): c/validate.c c/erc20.h c/ckb.h c/ckb_impl_libc.h c/schema/erc20_reader.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< -DCKB_IMPL_LIBC

c/schema/erc20_reader.h: c/erc20.fbs $(FLATCC)
	mkdir -p ./c/schema
	$(FLATCC) -c --reader -o ./c/schema ./c/erc20.fbs

$(VERIFY_BIN): c/verify.c $(SECP256K1_LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

$(FLATCC):
	cd deps/flatcc && scripts/build.sh

$(SECP256K1_LIB):
	cd deps/secp256k1 && \
		./autogen.sh && \
		CC=riscv64-unknown-elf-gcc LD=riscv64-unknown-elf-gcc ./configure --with-bignum=no --enable-ecmult-static-precomputation --enable-endomorphism --host=riscv64-elf && \
		make src/ecmult_static_pre_context.h src/ecmult_static_context.h

docker-build: $(VERIFY_BIN) $(VALIDATE_BIN) $(SPIKE_VALIDATE_BIN)

cargo:
	cargo build

clean:
	cd deps/secp256k1 && make clean
	cd deps/flatcc && ./scripts/cleanall.sh
	rm -rf $(ALWAYS_SUCCESS_BIN) $(VERIFY_BIN) $(VALIDATE_BIN) $(SPIKE_VALIDATE_BIN) c/schema

.PHONY: all clean cargo docker-build
