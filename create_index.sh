EXT=$1
OUTDIR=$2
COD="6"

[ -z "$EXT" ] && exit 1
[ -z "$OUTDIR" ] && exit 1

case "$EXT" in
  "nulc");;
  "rice")
    COD="0"
  ;;
  "s9")
    COD="4"
  ;;
  "s16")
    COD="5"
  ;;
  "trice")
    COD="3"
  ;;
  "vbyt")
    COD="2"
  ;;
esac

mkdir -p "$OUTDIR"

./build/bin/create_index 96 ./docs "$OUTDIR" $COD