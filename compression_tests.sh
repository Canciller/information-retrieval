COM=./build/bin/compress

OUT=./tests
IN=./datasets/datasets

N=$1
COD=$2
EXT=$3

[ -z "$N" -o -z "$COD" -o -z "$EXT" ] && exit 1

OUT="$OUT/$EXT"
mkdir -p "$OUT"

SUCC=0
FAIL=0

for i in $(seq $N); do
  INTEST="$IN/t$i"
  OUTTEST="$OUT/t$i.$EXT"

  INTXT="$INTEST.txt"
  OUTTXT="$OUTTEST.txt"

  $COM "$INTEST" "$OUTTEST" "$COD"
  if [ $? -ne 0 ]; then
    FAIL=$((FAIL + 1))
    echo "Fail: $INTEST"
  else
    SUCC=$((SUCC + 1))
  fi
  echo "-------------------------------"
done

echo "Failed: $FAIL"
echo "Success: $SUCC"