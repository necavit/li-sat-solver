#! /bin/bash

pushd `dirname $0` > /dev/null
SCRIPTPATH=`pwd`
popd > /dev/null

BIN_DIR=$(cd "$SCRIPTPATH/bin"; pwd)
BIN_NAME="sat"
BIN_FILE="$BIN_DIR/$BIN_NAME"
SAMPLES_DIR=$(cd "$SCRIPTPATH/sample_problems"; pwd)
OUTPUT_DIR=$(cd "$SCRIPTPATH/output"; pwd)

for FILE in $SAMPLES_DIR/*; do
  ./sat.sh profile $SAMPLES_DIR/`basename $FILE` $OUTPUT_DIR/`basename $FILE .cnf`.out
done
