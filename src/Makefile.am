MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = elemines

AM_CPPFLAGS = \
	      -DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
	      @ELEMENTARY_CFLAGS@

elemines_SOURCES = main.c

elemines_LDADD = @ELEMENTARY_LIBS@

EXTRA_DIST = elemines.h
