This script attempts to make a clean standalone project directory for the SiFive HiFive freedom-e-sdk.

First follow the instrutions at https://github.com/sifive/freedom-e-sdk to download and build the freedome-e-sdk.

Clone this repo (with --recursive) and run the ProjectCreator.sh script
```
git clone --recursive https://github.com/dbarbi1/HiFive-ProjectCreator.git 
chmod +x ProjectCreator.sh
./ProjectCreator -t /path/to/freedom-e-sdk /path/to/NewProject
```

Once the project is created, use the following make commands inside the project directory

```
make - builds project
make upload - uploads program to hifive board
make run_openocd
make run_debug - opens openocd and gdb for debug
make clean - cleans the build
```

The ProjectCreator -v switch adds Visual Studio Code tasks for the make commands as well as a Debug Launch Configuration. The Debug Launcher requires that "make run_openocd" is running (either from command line or VS Code Task)
