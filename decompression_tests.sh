DEC=./build/bin/decompress

OUT=./tests
IN=./tests
DATA=./datasets/txt

N=$1
COD=$2
EXT=$3

[ -z "$N" -o -z "$COD" -o -z "$EXT" ] && exit 1

OUT="$OUT/$EXT"
IN="$IN/$EXT"

SUCC=0
FAIL=0

for i in $(seq $N); do
  TST="t$i"
  DATATXT="$DATA/$TST.txt"
  INCOM="$IN/$TST.$EXT"
  OUTTXT="$INCOM.txt"

  RES=$($DEC "$INCOM" "$COD")
  if [ $? -ne 0 ]; then
    FAIL=$((FAIL + 1))
    echo "Failed: $INCOM"
    echo "--------------"
  else

    echo "$RES" > "$OUTTXT"
    if cmp --silent "$OUTTXT" "$DATATXT"; then
      echo "Success: $INCOM"
      echo "---------------"
      SUCC=$((SUCC + 1))
    else
      FAIL=$((FAIL + 1))
      echo "Failed: $INCOM"
      echo "--------------"
    fi
    rm "$OUTTXT"
  fi
done

echo "Failed: $FAIL"
echo "Success: $SUCC"