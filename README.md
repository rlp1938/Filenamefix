# Filenamefix

## What it's for

The object is to operate in a directory chosen by the user and alter all
file names in that dir to replace spaces in the names with underscores.
Any such names that have clusters of underscores will have that
 cluster replaced by a single underscore.

## To use it

* cd into the dir of interest

* filenamefix .

## To build it

1. cd into the dir where you cloned it.

2. ./build.sh

3. sudo cp _filenamefix_ _/usr/local/bin_

## Notes

This is just a quick and dirty fixer up for insane file names.

It should build on any Linux system but I am unsure of any thing else
 out there.
