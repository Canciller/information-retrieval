OUT=./tests_result
mkdir -p "$OUT"

N=$1

[ -z "$N" ] && exit 1

./compression_tests.sh "$N" 0 rice > "$OUT/compress.rice.txt"
#./compression_tests.sh "$N" 1 pfor > "$OUT/compress.pfor.txt"
./compression_tests.sh "$N" 2 vbyt > "$OUT/compress.vbyt.txt"
./compression_tests.sh "$N" 3 trice > "$OUT/compress.trice.txt"
./compression_tests.sh "$N" 4 s9 > "$OUT/compress.s9.txt"
./compression_tests.sh "$N" 5 s16 > "$OUT/compress.s16.txt"
./compression_tests.sh "$N" 6 nulc > "$OUT/compress.nulc.txt"