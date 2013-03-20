MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = elemines eleminesql

AM_CPPFLAGS = -DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
	      @ELEMINES_CFLAGS@

elemines_SOURCES = \
		   game.c \
		   gui.c \
		   init.c \
		   main.c

# QuickLaunch
eleminesql_SOURCES = $(elemines_SOURCES)
eleminesql_LTLIBRARIES = eleminesql.la
eleminesql_CFLAGS = -DELM_LIB_QUICKLAUNCH=1
eleminesql_LDFLAGS = @ELEMINES_LIBS@

eleminesqldir = $(libdir)
eleminesql_la_LIBADD = @ELEMINES_LIBS@
eleminesql_la_CFLAGS =
eleminesql_la_LDFLAGS = -module -avoid-version -no-undefined
eleminesql_la_SOURCES = $(elemines_SOURCES)


elemines_LDADD = @ELEMINES_LIBS@
elemines_LDFLAGS = $(LTLIBINTL)

localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

EXTRA_DIST = elemines.h gettext.h
