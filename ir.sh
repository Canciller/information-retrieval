EXT=$1
PHRASE=$2
COD="6"

[ -z "$EXT" ] && exit 1

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

if [ -z "$PHRASE" ]; then
  ./build/bin/ir "./index/$EXT" ./docs "$COD"
else
  echo "$PHRASE" | ./build/bin/ir "./index/$EXT" ./docs "$COD"
fi