#!/bin/bash
export DEST="./.exvim.ucos"
export TOOLS="/home/zswfox/.vim/tools/"
export FOLDERS="./bsp,./build,./h,./init,./lib,./port,./ucos,./usrApp"
export FILE_SUFFIXS=".*"
export TMP="${DEST}/_files"
export TARGET="${DEST}/files"
sh ${TOOLS}/shell/bash/update-filelist.sh
