#!/bin/bash
source $(dirname $0)/build_opt.sh
export REPODIR="$(pwd)"
export CFLAGS="$CFLAGS -I$REPODIR/build/include"
export LDFLAGS="$LDFLAGS -L$REPODIR/build/lib"

export TIZEN_CLI="$TIZEN_SDK/tools/ide/bin/tizen"
export PACKAGE="tpk"

cp build/lib/libnxml.so* lib/
cp build/lib/libmrss.so* lib/

$TIZEN_CLI build-native -a $TIZEN_ABI -c $CC -C $TYPE

rm lib/libmrss.so
rm lib/libmrss.so.0.*
rm lib/libnxml.so
rm lib/libnxml.so.0.*



$TIZEN_CLI package -t $PACKAGE -s $TIZEN_CERTIFICATE -- ./$TYPE

#make
echo "BUILD FINISHED"
