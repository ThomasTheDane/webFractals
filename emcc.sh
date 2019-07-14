#!/bin/bash

set -e

cd flam3
echo "============================================="
echo "emcc step"
echo "============================================="
emcc -O3 -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap", "FS"]' \
-s INVOKE_RUN=0 \
-g4 \
--bind \
-o flam3-render.js -s USE_LIBPNG=1 -I/Users/nattestad/workspaces/webFractals/libjpeg -s ALLOW_MEMORY_GROWTH=1 \
/Users/nattestad/workspaces/webFractals/flam3/flam3-render.o \
/Users/nattestad/workspaces/webFractals/flam3/flam3.o \
/Users/nattestad/workspaces/webFractals/flam3/interpolation.o \
/Users/nattestad/workspaces/webFractals/flam3/palettes.o \
/Users/nattestad/workspaces/webFractals/flam3/filters.o \
/Users/nattestad/workspaces/webFractals/flam3/parser.o \
/Users/nattestad/workspaces/webFractals/flam3/docstring.o \
/Users/nattestad/workspaces/webFractals/flam3/isaac.o \
/Users/nattestad/workspaces/webFractals/flam3/variations.o \
/Users/nattestad/workspaces/webFractals/flam3/jpeg.o \
/Users/nattestad/workspaces/webFractals/flam3/png.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jcapimin.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jcomapi.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jcparam.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jcapistd.o \
/Users/nattestad/workspaces/webFractals//libjpeg/jmemmgr.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jmemnobs.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jerror.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jdatadst.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jutils.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jcinit.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jccoefct.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jcmainct.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jcmaster.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jcprepct.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jccolor.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jcsample.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jcdctmgr.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jchuff.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jcmarker.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jfdctint.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jfdctflt.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jfdctfst.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jcarith.o \
/Users/nattestad/workspaces/webFractals/libjpeg/jaricom.o \
/Users/nattestad/workspaces/webFractals/libxml2/xzlib.o \
/Users/nattestad/workspaces/webFractals/libxml2/buf.o \
/Users/nattestad/workspaces/webFractals/libxml2/c14n.o \
/Users/nattestad/workspaces/webFractals/libxml2/catalog.o \
/Users/nattestad/workspaces/webFractals/libxml2/chvalid.o \
/Users/nattestad/workspaces/webFractals/libxml2/debugXML.o \
/Users/nattestad/workspaces/webFractals/libxml2/dict.o \
/Users/nattestad/workspaces/webFractals/libxml2/encoding.o \
/Users/nattestad/workspaces/webFractals/libxml2/entities.o \
/Users/nattestad/workspaces/webFractals/libxml2/error.o \
/Users/nattestad/workspaces/webFractals/libxml2/globals.o \
/Users/nattestad/workspaces/webFractals/libxml2/hash.o \
/Users/nattestad/workspaces/webFractals/libxml2/HTMLparser.o \
/Users/nattestad/workspaces/webFractals/libxml2/HTMLtree.o \
/Users/nattestad/workspaces/webFractals/libxml2/legacy.o \
/Users/nattestad/workspaces/webFractals/libxml2/list.o \
/Users/nattestad/workspaces/webFractals/libxml2/nanoftp.o \
/Users/nattestad/workspaces/webFractals/libxml2/nanohttp.o \
/Users/nattestad/workspaces/webFractals/libxml2/parser.o \
/Users/nattestad/workspaces/webFractals/libxml2/parserInternals.o  \
/Users/nattestad/workspaces/webFractals/libxml2/pattern.o \
/Users/nattestad/workspaces/webFractals/libxml2/relaxng.o \
/Users/nattestad/workspaces/webFractals/libxml2/SAX.o \
/Users/nattestad/workspaces/webFractals/libxml2/SAX2.o \
/Users/nattestad/workspaces/webFractals/libxml2/schematron.o \
/Users/nattestad/workspaces/webFractals/libxml2/testdso.o \
/Users/nattestad/workspaces/webFractals/libxml2/threads.o \
/Users/nattestad/workspaces/webFractals/libxml2/tree.o \
/Users/nattestad/workspaces/webFractals/libxml2/uri.o \
/Users/nattestad/workspaces/webFractals/libxml2/valid.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlregexp.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlsave.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlschemas.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlschemastypes.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlstring.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlunicode.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlwriter.o \
/Users/nattestad/workspaces/webFractals/libxml2/xpath.o \
/Users/nattestad/workspaces/webFractals/libxml2/xpointer.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlsave.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlschemas.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlschemastypes.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlstring.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlunicode.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlwriter.o \
/Users/nattestad/workspaces/webFractals/libxml2/xpath.o \
/Users/nattestad/workspaces/webFractals/libxml2/xpointer.o \
/Users/nattestad/workspaces/webFractals/libxml2/xinclude.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlmemory.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlmodule.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlreader.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlregexp.o \
/Users/nattestad/workspaces/webFractals/libxml2/xmlIO.o \
/Users/nattestad/workspaces/webFractals/libxml2/xlink.o \
-s MODULARIZE=1 \
-s EXPORT_ES6=1 \



# # Create output folder
# cd ..
# mkdir -p dist
# # Move artifacts
# mv flam3/flam3-render.{js,wasm} dist