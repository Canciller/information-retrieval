OUT=./tests_result
mkdir -p "$OUT"

N=$1

[ -z "$N" ] && exit 1

./decompression_tests.sh "$N" 0 rice > "$OUT/decompress.rice.txt"
#./decompression_tests.sh "$N" 1 pfor > "$OUT/decompress.pfor.txt"
./decompression_tests.sh "$N" 2 vbyt > "$OUT/decompress.vbyt.txt"
./decompression_tests.sh "$N" 3 trice > "$OUT/decompress.trice.txt"
./decompression_tests.sh "$N" 4 s9 > "$OUT/decompress.s9.txt"
./decompression_tests.sh "$N" 5 s16 > "$OUT/decompress.s16.txt"
./decompression_tests.sh "$N" 6 nulc > "$OUT/decompress.nulc.txt"