MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = default.edc

SUBDIRS = fonts images sounds

EDJE_CC = edje_cc
EDJE_FLAGS = \
	     -v \
	     -id $(top_srcdir)/data/themes/images \
	     -fd $(top_srcdir)/data/themes/fonts \
	     -sd $(top_srcdir)/data/themes/sounds

filesdir = $(datadir)/$(PACKAGE)/themes
files_DATA = default.edj

default.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default.edc \
	$(top_builddir)/data/themes/default.edj

clean-local:
	rm -f *.edj
	rm -rf autom4te.cache

