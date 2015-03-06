#! /bin/bash

pushd `dirname $0` > /dev/null
SCRIPTPATH=`pwd`
popd > /dev/null

SAMPLES_DIR=$(cd "$SCRIPTPATH/sample_problems"; pwd)
OUTPUT_DIR=$(cd "$SCRIPTPATH/output_ pico"; pwd)

for FILE in $SAMPLES_DIR/*; do
  echo "Processing `basename $FILE` ..."
  /usr/bin/time -f "%e" picosat -v $SAMPLES_DIR/`basename $FILE` &> $OUTPUT_DIR/`basename $FILE .cnf`.out.pico
done
