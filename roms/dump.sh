#!/bin/bash

if [[ $# -eq 0 ]]; then
    echo "Usage: $0 <rom_file>"
    exit -1
fi

od --endian=big -An -vtx2 -w1 $1 | cut -c2-
