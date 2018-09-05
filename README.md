# README

Note this project requires you to have [riscv-tools](https://github.com/riscv/riscv-tools) compiled with rv32imac architecture in your path. You can either compile it from source, or use the docker solution described later in this file to build necessary RISC-V files, then run the remaining Rust binaries natively.

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
