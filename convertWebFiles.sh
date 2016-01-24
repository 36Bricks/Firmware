#!/bin/bash

echo "Converting files in folder \"FlashFiles\" to C Header file flashfiles.h"

WEBSITE="FlashFiles/"
CURRDIR="$(pwd)"
OUTFILE="$CURRDIR/flashfiles.h"

#change into website folder
cd $WEBSITE

cat > $OUTFILE <<DELIMITER
//
// converted websites to mainly flash variables
//

#include "Flash.h"

DELIMITER

#convert contents into array of bytes
INDEX=0
for i in $(ls -1); do
  CONTENT=$(cat $i | xxd -i)
  printf "FLASH_ARRAY(uint8_t, file_$INDEX,\n$CONTENT\n);\n" >> $OUTFILE
  echo >> $OUTFILE
  INDEX=$((INDEX+1))
done

# write typedefinition
cat >> $OUTFILE <<DELIMITER
struct t_websitefiles {
  const char* filename;
  const char* mime;
  const unsigned int len;
  const _FLASH_ARRAY<uint8_t>* content;
} files[] = {
DELIMITER

# add other data and create array
INDEX=0
for i in $(ls -1); do
  CONTENT=$(cat $i | xxd -i)
  CONTENT_LEN=$(echo $CONTENT | grep -o '0x' | wc -l)  
  MIMETYPE=$(file --mime-type -b $i)
  
  echo "  {" >> $OUTFILE
  echo "    .filename = \"$i\"," >> $OUTFILE
  echo "    .mime = \"$MIMETYPE\"," >> $OUTFILE
  echo "    .len = $CONTENT_LEN," >> $OUTFILE
  echo "    .content = &file_$INDEX" >> $OUTFILE
  echo "  }," >> $OUTFILE

  INDEX=$((INDEX+1))
done
echo "};" >> $OUTFILE

cd $CURRDIR