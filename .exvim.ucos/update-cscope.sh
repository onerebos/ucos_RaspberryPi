#!/bin/bash
export DEST="./.exvim.ucos"
export TOOLS="/home/zswfox/.vim/tools/"
export CSCOPE_CMD="cscope"
export OPTIONS="-kb -i"
export TMP="./cscope.out"
export TARGET="${DEST}/cscope.out"
sh ${TOOLS}/shell/bash/update-cscope.sh
