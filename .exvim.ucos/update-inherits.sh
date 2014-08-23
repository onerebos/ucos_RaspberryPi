#!/bin/bash
export DEST="./.exvim.ucos"
export TOOLS="/home/zswfox/.vim/tools/"
export TMP="${DEST}/_inherits"
export TARGET="${DEST}/inherits"
sh ${TOOLS}/shell/bash/update-inherits.sh
