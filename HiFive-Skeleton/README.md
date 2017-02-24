This Project attempts to make a clean standalone project directory for the SiFive HiFive freedom-e-sdk.

First follow the instrutions at https://github.com/sifive/freedom-e-sdk to download and build the freedome-e-sdk.

Then clone this repo and change the TARGET and TOOL_DIR variables in the makefile. TARGET is the name of the resulting elf file. TOOL_DIR is the directory of the built Freedom-e-sdk.

make - builds project
make upload - uploads program to hifive board
make run_debug - opens openocd and gdb for debug