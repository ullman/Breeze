AC_INIT([breeze], [0.1.0], [henrik@ullman.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([
                  Makefile
                   src/Makefile
                   inc/Makefile
                   res/edje/Makefile
                   ])
PKG_CHECK_MODULES([BREEZE], [ecore ecore-evas edje glib-2.0 elementary sqlite3 nxml
                   mrss ewebkit2])
AC_OUTPUT
