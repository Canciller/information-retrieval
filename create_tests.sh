EX=./build/bin/create_test
OUT=./datasets/datasets
OUTXT=./datasets/txt
N=$1

mkdir -p "$OUT"
mkdir -p "$OUTXT"

[ -z "$N" ] && exit 1

for i in $(seq $N);do
  OUTPATH="$OUT/t$i"
  OUTPATHTXT="$OUTXT/t$i.txt"
  $EX "$OUTPATH" "$i" > "$OUTPATHTXT"
done