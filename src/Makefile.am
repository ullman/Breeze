#AM_CPPFLAGS = -I$(top_srcdir)/inc
AM_CPPFLAGS = -DDATADIR=\"$(datadir)\"
bin_PROGRAMS = breeze
breeze_SOURCES = app_control_functions.c \
				 database.c \
				 breeze.c \
				 rss_functions.c \
				 cb_functions.c
breeze_CPPFLAGS = $(BREEZE_CFLAGS) -I$(top_srcdir)/inc -DDATADIR=\"$(datadir)\"
breeze_LDFLAGS= $(BREEZE_LIBS)
