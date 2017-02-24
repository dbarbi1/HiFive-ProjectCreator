#!/bin/bash

function message    {
    echo " "
    echo "Usage ./ProjectCreator.sh [options] DirToProject/NewProject"
    echo " "
    echo "options:"
    echo "-t, Path to SDK Directory. Example: -t /path/to/freedom-e-sdk"
    echo "    Default behavior uses ../ as the sdk directory."
    echo "-b, Specifies the board to use when building the BSP"
    echo "    Default behavior uses freedom-e300-hifive1"
    echo "-v, Add Visual Studio Code Debugger and Launch files"
    exit 1
}

if [ $# -eq 0 ];    then
    message
else
    SHIFTER=0
    while getopts 't:b:v' flag; do
        case "${flag}" in
            t) SDK_DIR=${OPTARG}; CURDIR=`pwd`; cd $SDK_DIR; SDK_DIR=`pwd`; cd $CURDIR; SHIFTER=$((SHIFTER+2));;
            b) BOARD=${OPTARG}; SHIFTER=$((SHIFTER+2));;
            v) VSCODE=true; SHIFTER=$((SHIFTER+1));;
            ?) message; exit 2 ;;
        esac
    done
    shift $SHIFTER
fi

#DEST_DIR is always $1
DEST_DIR=$1

##set defaults if not set
if [ -z ${PROJ_NAME} ]; then 
    PROJ_NAME="$(basename $1)"
fi

if [ -z ${SDK_DIR} ]; then
    SDK_DIR="$(dirname `pwd`)"
fi

if [ -z ${BOARD} ]; then
    BOARD=freedom-e300-hifive1
fi

echo " "
echo Creating new project in $DEST_DIR
echo Project Name is $PROJ_NAME
echo SDK directory is $SDK_DIR
echo Board is $BOARD

#test if dest dir exist
#if so check $2 if we should overwrite
if [ -d "$DEST_DIR" ]; then
    echo "$DEST_DIR already exist"
    exit 2
fi

mkdir $DEST_DIR
mkdir $DEST_DIR/docs

cp Makefile $DEST_DIR
cp README.md $DEST_DIR
cp -r HiFive-Skeleton/src/ $DEST_DIR/src
cp -r $SDK_DIR/bsp $DEST_DIR/bsp
rm $DEST_DIR/bsp/env/common.mk
cp bsp.mk $DEST_DIR/bsp/env/

##bsp/tools/openocd_upload.sh
rm $DEST_DIR/bsp/tools/openocd_upload.sh
cp openocd_upload.sh $DEST_DIR/bsp/tools/

if [ "$VSCODE" = true ]; then  
    cp -r vscode $DEST_DIR/.vscode 
fi

##now change variables variables

##Makefile TARGET and TOOL_DIR and BOARD
cd $DEST_DIR
sed -i.bak "s|@@TARGET|$PROJ_NAME|g" Makefile
sed -i.bak "s|@@TOOL_DIR|$SDK_DIR|g" Makefile
sed -i.bak "s|@@BOARD|$BOARD|g" Makefile
rm Makefile.bak

if [ "$VSCODE" = true ]; then  
    sed -i.bak "s|@@VSDEBUGPATH|\"$SDK_DIR/toolchain/bin/riscv32-unknown-elf-gdb\"|g" .vscode/launch.json
    rm .vscode/launch.json.bak
fi
##

#cd $1 && make
