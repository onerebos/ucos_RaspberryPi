#!/bin/bash
export DEST="./.exvim.ucos"
export TOOLS="/home/pi/.vim/tools/"
export EXCLUDE_FOLDERS=""
export TMP="${DEST}/_ID"
export TARGET="${DEST}/ID"
sh ${TOOLS}/shell/bash/update-idutils.sh
