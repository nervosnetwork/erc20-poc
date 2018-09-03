CC := riscv32-unknown-elf-gcc
CFLAGS := -Ideps/secp256k1/include
SECP256K1_LIB := deps/secp256k1/.libs/libsecp256k1.a
VERIFY_BIN := build/verify

all: $(VERIFY_BIN)

$(VERIFY_BIN): c/verify.c $(SECP256K1_LIB)
	$(CC) $(CFLAGS) -o $@ $^

$(SECP256K1_LIB):
	cd deps/secp256k1 && \
		./autogen.sh && \
		CC=riscv32-unknown-elf-gcc LD=riscv32-unknown-elf-gcc ./configure --with-bignum=no --enable-ecmult-static-precomputation --enable-endomorphism --host=riscv32-elf && \
		make

clean:
	cd deps/secp256k1 && make clean
	rm $(VERIFY_BIN)

.PHONY: all clean
