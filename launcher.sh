#!/usr/bin/env bash
set -e

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <num_decrypters>"
  exit 1
fi

NUM_DECRYPTERS="$1"
if ! [[ "$NUM_DECRYPTERS" =~ ^[1-9][0-9]*$ ]]; then
  echo "Error: num_decrypters must be a positive integer."
  exit 1
fi

HOST_MTA_DIR="/tmp/mtacrypt"

docker build -t mta_crypt_encrypter -f encrypter/Dockerfile .
docker build -t mta_crypt_decrypter -f decrypter/Dockerfile .

sudo rm -rf "$HOST_MTA_DIR"
mkdir -p "$HOST_MTA_DIR"
sudo chown "$USER":"$USER" "$HOST_MTA_DIR"
chmod 777 "$HOST_MTA_DIR"

cp mtacrypt.conf "$HOST_MTA_DIR"/
chmod 666 "$HOST_MTA_DIR"/mtacrypt.conf

ENCRYPTER_ID=$(docker run -d \
  -v "$HOST_MTA_DIR":/mnt/mta \
  mta_crypt_encrypter)

echo "Encrypter (${ENCRYPTER_ID}) starting…"


for i in $(seq 1 "$NUM_DECRYPTERS"); do
  CID=$(docker run -d \
    -v "$HOST_MTA_DIR":/mnt/mta \
    mta_crypt_decrypter \
    /root/decrypter)
    
  echo "Decrypter #${i} (${CID}) starting..."
done

echo "System initialization finished successfully with ${NUM_DECRYPTERS} decrypters."

