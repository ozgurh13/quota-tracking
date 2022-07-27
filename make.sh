#!/bin/sh

cd src

# make the C side
echo -n "making project..."
gcc -Wall -Wextra -O3 main.c msleep.c hashmap.c database.c           \
    extract.c proclist.c proc.c open_device.c fifo.c parser.c        \
    -o ../main -lpcap -lpthread -lsqlite3
echo " done"


# make the shell
#   redirect stdout to /dev/null, don't touch stderr
echo -n "making shell..."
ghc -Wall -Wextra -O2 --make Shell.hs -o ../shell -outputdir /tmp  1>/dev/null
echo " done"

