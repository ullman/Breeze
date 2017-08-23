#!/bin/bash
source $(dirname $0)/build_opt.sh
#get library sources
if [ ! -f libmrss.tar.gz ]
then
    wget -O libmrss.tar.gz https://github.com/ullman/libmrss/archive/6057dbca1113711123c45754547e73a5acae7d08.tar.gz
fi
if [ ! -f libnxml-0.18.3.tar.gz ]
then
    wget http://www.autistici.org/bakunin/libnxml/libnxml-0.18.3.tar.gz
fi

#architecture
if [ $TIZEN_ABI == 'arm' ]
then
    export CC="$HOME/tizen-studio/tools/arm-linux-gnueabi-gcc-4.9/bin/arm-linux-gnueabi-gcc"
    export HOST="arm-linux-gnueabi"
    export SYSROOT="--sysroot=$HOME/tizen-studio/platforms/tizen-2.4/mobile/rootstraps/mobile-2.4-device.core"
fi
if [ $TIZEN_ABI == 'x86' ]
then
    export CC="$HOME/tizen-studio/tools/i386-linux-gnueabi-gcc-4.9/bin/i386-linux-gnueabi-gcc"
    export HOST="i386-linux-gnueabi"
    export SYSROOT="--sysroot=$HOME/tizen-studio/platforms/tizen-2.4/mobile/rootstraps/mobile-2.4-emulator.core"
fi

#build libnxml
export TARGET="$(gcc -dumpmachine)"
echo $TARGET
export REPODIR="$(pwd)"
export CFLAGS="$CFLAGS -I$REPODIR/build/include"
export LDFLAGS="$LDFLAGS -L$REPODIR/build/lib"
export PKG_CONFIG_PATH="$REPODIR/build/lib"

mkdir lib
mkdir build

cd build

tar xf ../libnxml-0.18.3.tar.gz
cd libnxml-0.18.3
CC="$CC $SYSROOT" ./configure --host=$HOST --build=$TARGET --prefix=$REPODIR/build

make install
cd ..
cd ..

cd build
tar xf ../libmrss.tar.gz
cd libmrss-6057dbca1113711123c45754547e73a5acae7d08
CC="$CC $SYSROOT" ./configure --host=$HOST --build=$TARGET --prefix=$REPODIR/build

make install
cd ..
cd ..

