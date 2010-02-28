#!/bin/sh

# clean.sh
# Cleans autotools files in the gnuclad source tree, in order to fix broken
# autotools behaviour.
# This file is of no interest to the end user


#make clean


# UNDO ./autoreconf -i
# 
rm -rf autom4te.cache
rm -f aclocal.m4
rm -f config.h.in
rm -f configure depcomp install-sh missing
rm -f Makefile.in src/Makefile.in manual/Makefile.in
rm -f manual/mdate-sh


# UNDO ./configure
#
rm -f config.log config.status
rm -f stamp-h1
#rm -f config.h
#rm -f Makefile src/Makefile manual/Makefile
rm -rf src/.deps


# UNDO make html  !!!  necessary in order to build the manual correctly
#
rm -f manual/src/stamp-vti manual/src/.dirstamp
