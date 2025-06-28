#!/bin/bash

if [ -z "$1" ]; then
    echo "Usage: $0 <number_of_blocks>"
    exit 1
fi

COUNT=$1
OUTDIR="$HOME/Ex1/data"
mkdir -p "$OUTDIR"
current_hash=$(wget -qO- https://api.blockcypher.com/v1/btc/main | awk -F'"' '/"hash"/ {print $4; exit}')

for (( i=0; i<$COUNT; i++ ))
do
   
 
    block_data=$(wget -qO- "https://api.blockcypher.com/v1/btc/main/blocks/$current_hash")

    hash=$(echo "$block_data" | awk -F'"' '/"hash"/ {print $4; exit}')
    height=$(echo "$block_data" | awk -F':' '/"height"/ {gsub(/,/, "", $2); print $2; exit}' | sed 's/^[ \t]*//;s/[ \t]*$//')
    total=$(echo "$block_data" | awk -F':' '/"total"/ {gsub(/,/, "", $2); print $2; exit}' | sed 's/^[ \t]*//;s/[ \t]*$//')
    time=$(echo "$block_data" | awk -F'"' '/"time"/ {print $4; exit}')
    relayed_by=$(echo "$block_data" | awk -F'"' '/"relayed_by"/ {print $4; exit}')
    prev_block=$(echo "$block_data" | awk -F'"' '/"prev_block"/ {print $4; exit}')

    {
      echo "hash: $hash"
      echo "height: $height"
      echo "total: $total"
      echo "time: $time"
      echo "relayed_by: $relayed_by"
      echo "prev_block: $prev_block"
    } > "$OUTDIR/block_$i.txt"

    current_hash=$prev_block
done

