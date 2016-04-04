#!/bin/sh


echo "Starting thiolsTemp app"

echo "Creating scanning thread"
sudo hcitool -i hci0 lescan --passive --duplicates >> /dev/null &

echo "Creating parser scanning thread"
sudo hcidump -i ^Ci0 -R |./parser/bleParser

