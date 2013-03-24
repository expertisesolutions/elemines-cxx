MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = default.edc default-big.edc

SUBDIRS = fonts images

EDJE_CC = @edje_cc@
EDJE_FLAGS = \
	     -id $(top_srcdir)/data/themes/images \
	     -fd $(top_srcdir)/data/themes/fonts

filesdir = $(datadir)/$(PACKAGE)/themes
files_DATA = default.edj default-big.edj

default.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default.edc \
	$(top_builddir)/data/themes/default.edj

default-big.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default-big.edc \
	$(top_builddir)/data/themes/default-big.edj

clean-local:
	rm -f *.edj
	rm -rf autom4te.cache

