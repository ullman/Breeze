TIZEN_CLI = $(TIZEN_SDK)/tools/ide/bin/tizen
CC = llvm
TYPE = Debug
PACKAGE = tpk

all: build

build:
	./build_all.sh

clean:
	rm -rf ./.sign
	rm -rf ./Release
	rm -rf ./Debug
	rm -rf ./lib
	rm -rf ./build
