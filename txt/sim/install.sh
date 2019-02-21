#!/bin/bash

# make sure destination folder exists
if [ ! -d "/tmp/jsimfiles/"  ]; then
    mkdir /tmp/jsimfiles
fi

# copy all relevant files to global location
files=( `ls` )
for f in "${files[@]}"; do
    if [ ! $f = "install.sh" ]; then
        # overwrite it if ti exists
        mv $f /tmp/jsimfiles/$f
    fi
done
