#!/bin/bash

BASE="./index.gov"

./create_index.sh nulc "$BASE/nulc"
./create_index.sh rice "$BASE/rice"
./create_index.sh trice "$BASE/trice"
./create_index.sh s9 "$BASE/s9"
./create_index.sh s16 "$BASE/s16"
./create_index.sh vbyt "$BASE/vbyt"