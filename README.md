# README

Note this project requires you to have [riscv-tools](https://github.com/riscv/riscv-tools) compiled with rv32imac architecture in your path. You can either compile it from source, or use the docker solution described later in this file to build necessary RISC-V files, then run the remaining Rust binaries natively.

## How to verify secp256k1 signature

```bash
# First build everything
$ git submodule update --init
$ make
# Create a private key first
$ ./target/debug/private_key_generator
cff47b76e6ad9836dc7c4b1826b57b8e6c755aea1e16d7ee0801c26df5e2916d
# Use the private key to sign a message
$ ./target/debug/secp256k1_signer cff47b76e6ad9836dc7c4b1826b57b8e6c755aea1e16d7ee0801c26df5e2916d foo bar
Pubkey: 0326b0a7a88857d2d802c1b286a021eac8f7c13f8f5b4a2d1e4af4c6829903648f
Signature: 3045022100a2fe2d3ea288542f1bfc85369828cd357e970437f05e0e1f121d477762105a1c02206648ee22e2ef5eb9ee63e0454f977a1c48cb706fc5db7f70fc0237b7905c0343
# Note all the arguments after the private key is included in the signature calculation, in this case, `foo` and `bar` are used

# Now we can verify a signature
$ spike pk build/verify 0326b0a7a88857d2d802c1b286a021eac8f7c13f8f5b4a2d1e4af4c6829903648f 0326b0a7a88857d2d802c1b286a021eac8f7c13f8f5b4a2d1e4af4c6829903648f foo bar
# Return code of 0 means validation succeeds
$ echo $?
0

# We can also try verifying an invalid signature
$ spike pk build/verify 0326b0a7a88857d2d802c1b286a021eac8f7c13f8f5b4a2d1e4af4c6829903648f 3045022100a2fe2d3ea288542f1bfc85369828cd357e970437f05e0e1f121d477762105a1c02206648ee22e2ef5eb9ee63e0454f977a1c48cb706fc5db7f70fc0237b7905c0343 foo bar invalid part
$ echo $?
2
# We can see this fails
```

## How to validate ERC20 contract

```bash
# First build everything
$ git submodule update --init
$ make
# Create a few folders for cell mock data
$ mkdir -p cells/input1
$ mkdir -p cells/output1
# Create input cell data
$ ./target/debug/cell_creator cells/input1/data
Enter owner address:
owner123
Enter total supply:
1000000000
Enter balance address(empty string to finish entering):
abc
Enter address tokens:
1000
Enter balance address(empty string to finish entering):
def
Enter address tokens:
2000
Enter balance address(empty string to finish entering):

Enter allowed address(empty string to finish entering):

# Create output cell data
$ ./target/debug/cell_creator cells/output1/data
Enter owner address:
owner123
Enter total supply:
1000000000
Enter balance address(empty string to finish entering):
abc
Enter address tokens:
500
Enter balance address(empty string to finish entering):
def
Enter address tokens:
2000
Enter balance address(empty string to finish entering):
zxc
Enter address tokens:
500
Enter balance address(empty string to finish entering):

Enter allowed address(empty string to finish entering):

# Now create argv data for input cell to encode commands:
$ ./target/debug/argv_creator cells/input1 validate pubkey signature hash transfer abc zxc 500
# Mock cell data is now completed:
$ tree cells
cells
├── input1
│   ├── argc
│   ├── argv0
│   ├── argv1
│   ├── argv2
│   ├── argv3
│   ├── argv4
│   ├── argv5
│   ├── argv6
│   ├── argv7
│   └── data
└── output1
    └── data

2 directories, 11 files
# Run the validate script
$ spike pk ./build/validate 0 1 input1 1 output1
# Return code of 0 means that validation succeeds
$ echo $?
0

# We can create an error output as well and try to validate that:
$ mkdir -p cells/error-output1
$ ./target/debug/cell_creator cells/error-output1/data
Enter owner address:
owner123
Enter total supply:
1000000000
Enter balance address(empty string to finish entering):
abc
Enter address tokens:
1000
Enter balance address(empty string to finish entering):
def
Enter address tokens:
2000
Enter balance address(empty string to finish entering):
zxc
Enter address tokens:
500
Enter balance address(empty string to finish entering):

Enter allowed address(empty string to finish entering):

# This time it won't work:
$ spike pk ./build/validate 0 1 input1 1 error-output1
$ echo $?
250
```

## Docker solution

```bash
$ sudo docker run -it --rm -v `pwd`:/code xxuejie/docker-rv32imac bash
(docker) $ cd /code
(docker) $ apt-get update && apt-get install -y git autoconf automake autotools-dev curl libmpc-dev libmpfr-dev libgmp-dev libusb-1.0-0-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev device-tree-compiler pkg-config libexpat-dev cmake ninja-build && apt-get clean
(docker) $ make docker-build
(docker) $ exit
```

Now you will have 2 RISC-V `build/verify` and `build/validate` compiled and ready to use. You can continue to compile Rust binaries in your native environment:

```bash
$ cargo build
```

Then continue playing with this project.
