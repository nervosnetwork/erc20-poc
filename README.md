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
