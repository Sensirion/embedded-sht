#!/bin/bash

if [ "$#" -ne 1 ] || ! [ -d "$1" ]; then
  echo "Usage: $0 OUTPUT_DIRECTORY" >&2
  exit 1
fi

BASE_DIR=$(dirname "$0")

"${BASE_DIR}/copy_shtc1_driver.sh"

# Copy everything
cp -r "${BASE_DIR}/"* "$1"
# Delete unneeded files
rm "$1"/copy_shtc1_driver.sh
rm "$1"/prepare_release.sh
rm "$1"/README.md
