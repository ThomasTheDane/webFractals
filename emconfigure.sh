#!/bin/bash

set -e

cd flam3
make clean
echo "============================================="
echo "emconfigure step"
echo "============================================="
LDFLAGS='-s USE_ZLIB=1 -s USE_LIBPNG=1 -L/Users/nattestad/workspaces/webFractals/libjpeg/.libs -L/Users/nattestad/workspaces/webFractals/libxml2/.libs' CPPFLAGS='-I/Users/nattestad/workspaces/webFractals/libjpeg -s USE_LIBPNG=1' emconfigure ./configure
