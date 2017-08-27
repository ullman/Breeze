TIZEN_CLI = $(TIZEN_SDK)/tools/ide/bin/tizen
PACKAGE = tpk
TARGET_DIR= build
TARGET= $(TARGET_DIR)/breeze
BINDIR = /usr/bin/
CC	= gcc
SRCDIR = src
CFLAGS = -c -O2 -Wall -Iinc
LDFLAGS =
XCFLAGS = `pkg-config --cflags ecore ecore-evas glib-2.0 elementary`
XLDFLAGS = `pkg-config --libs ecore ecore-evas glib-2.0 elementary sqlite3 nxml mrss`
SOURCES= $(SRCDIR)/cb_functions.c $(SRCDIR)/rss_functions.c $(SRCDIR)/breeze.c $(SRCDIR)/database.c $(SRCDIR)/app_control_functions.c

OBJECTS=$(SOURCES:.c=.o)

$(TARGET): $(SRCDIR)/cb_functions.o $(SRCDIR)/rss_functions.o $(SRCDIR)/breeze.o $(SRCDIR)/database.o $(SRCDIR)/app_control_functions.o
	mkdir -p $(TARGET_DIR)
	$(CC) $(LDFLAGS) $(XLDFLAGS) $(SRCDIR)/cb_functions.o $(SRCDIR)/rss_functions.o $(SRCDIR)/breeze.o $(SRCDIR)/database.o $(SRCDIR)/app_control_functions.o -o $@

.c.o:
	$(CC) $(CFLAGS) $(XCFLAGS) $< -o $@

all: tizen linux

tizen:
	./build_all.sh

linux: $(TARGET)

install: all
	install -D $(TARGET) $(BINDIR)/$(TARGET)

uninstall:
	  -rm $(BINDIR)/$(TARGET)


clean:
	rm -rf ./.sign
	rm -rf ./Release
	rm -rf ./Debug
	rm -rf ./lib
	rm -rf ./build
	rm -f $(TARGET)
	rm -f $(OBJECTS)




.PHONY: all clean
