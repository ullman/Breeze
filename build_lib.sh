#!/bin/bash
source $(dirname $0)/build_opt.sh
#get library sources
if [ ! -f libmrss-stripped.tar.gz ]
then
    wget -O libmrss-stripped.tar.gz https://github.com/moraxy/libmrss-stripped/archive/42db00201dc9229b893dc8a2cf9304867d7bb96a.tar.gz
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
tar xf ../libmrss-stripped.tar.gz
cd libmrss-stripped-42db00201dc9229b893dc8a2cf9304867d7bb96a
CC="$CC $SYSROOT" ./configure --host=$HOST --build=$TARGET --prefix=$REPODIR/build

make install
cd ..
cd ..

