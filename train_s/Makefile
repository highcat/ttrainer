include makeinclude

DIRS	=	$(IMAGEDIRS) src fluid test documentation

all: makeinclude
	for dir in $(DIRS); do\
		echo "=== making $$dir ===";\
		(cd $$dir; $(MAKE) $(MFLAGS)) || break;\
	done

install: makeinclude
	-mkdir -p $(DESTDIR)$(bindir)
	$(RM) $(DESTDIR)$(bindir)/fltk-config
	-cp fltk-config $(DESTDIR)$(bindir)
	-chmod 755 $(DESTDIR)$(bindir)/fltk-config
	for dir in FL $(DIRS); do\
		echo "=== installing $$dir ===";\
		(cd $$dir; $(MAKE) $(MFLAGS) install) || break;\
	done

install-desktop: makeinclude
	cd documentation; $(MAKE) $(MFLAGS) $(INSTALL_DESKTOP)
	cd fluid; $(MAKE) $(MFLAGS) $(INSTALL_DESKTOP)
	cd test; $(MAKE) $(MFLAGS) $(INSTALL_DESKTOP)

uninstall: makeinclude
	$(RM) $(DESTDIR)$(bindir)/fltk-config
	for dir in FL $(DIRS); do\
		echo "=== uninstalling $$dir ===";\
		(cd $$dir; $(MAKE) $(MFLAGS) uninstall) || break;\
	done

uninstall-desktop: makeinclude
	cd documentation; $(MAKE) $(MFLAGS) $(UNINSTALL_DESKTOP)
	cd fluid; $(MAKE) $(MFLAGS) $(UNINSTALL_DESKTOP)
	cd test; $(MAKE) $(MFLAGS) $(UNINSTALL_DESKTOP)

depend: makeinclude
	for dir in $(DIRS); do\
		echo "=== making dependencies in $$dir ===";\
		(cd $$dir; $(MAKE) $(MFLAGS) depend) || break;\
	done

clean:
	-$(RM) core *.o
	for dir in $(DIRS); do\
		echo "=== cleaning $$dir ===";\
		(cd $$dir; $(MAKE) $(MFLAGS) clean) || break;\
	done

distclean: clean
	$(RM) config.*
	$(RM) fltk-config fltk.list makeinclude
	$(RM) FL/Makefile
	$(RM) documentation/*.$(CAT1EXT)
	$(RM) documentation/*.$(CAT3EXT)
	$(RM) documentation/fltk.pdf
	$(RM) documentation/fltk.ps
	$(RM) -r documentation/fltk.d
	for file in test/*.fl; do\
		$(RM) test/`basename $file .fl`.cxx; \
		$(RM) test/`basename $file .fl`.h; \
	done

makeinclude: configure configh.in makeinclude.in
	if test -f config.status; then \
		./config.status --recheck; \
		./config.status; \
	else \
		./configure; \
	fi
	touch config.h
	chmod +x fltk-config

configure: configure.in
	autoconf

portable-dist:
	epm -v -s fltk.xpm fltk

native-dist:
	epm -v -f native fltk


#
# End of "$Id: Makefile 4756 2006-01-15 18:36:16Z mike $".
#
