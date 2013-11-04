MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = elemines

AM_CPPFLAGS = -DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
	      @ELEMINES_CFLAGS@

elemines_SOURCES = \
		   game.c \
		   gui.c \
		   init.c \
		   main.c

elemines_CFLAGS = -fpie
elemines_LDADD = @ELEMINES_LIBS@
elemines_LDFLAGS = $(LTLIBINTL) -pie -rdynamic

localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

EXTRA_DIST = elemines.h gettext.h
