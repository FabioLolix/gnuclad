#!/bin/sh

# build.sh
# Build helper for gnuclad.
# This file is of no interest to the end user.

autoreconf -i -Wall
./configure

make info
make html
