MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = elemines

AM_CPPFLAGS = -DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
	      @ELEMINES_CFLAGS@

elemines_SOURCES = \
		   game.cpp \
		   gui.cpp \
		   main.cpp \
                   getopt.c

elemines_LDADD = @ELEMINES_LIBS@
elemines_LDFLAGS = $(LTLIBINTL)

localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

EXTRA_DIST = elemines.h gettext.h
