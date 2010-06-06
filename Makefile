# Automatically Generated Makefile by EDE.
# For use with: make
#
# DO NOT MODIFY THIS FILE OR YOUR CHANGES MAY BE LOST.
# EDE is the Emacs Development Environment.
# http://cedet.sourceforge.net/ede.shtml
#

top="$(CURDIR)"/
ede_FILES=Project.ede Makefile

Debug_SOURCES=block.h block.cpp main.cpp radix.cpp mtf.cpp rle.cpp bwt.cpp
Debug_OBJ= block.o main.o radix.o mtf.o rle.o bwt.o
CXX= g++
CPPFLAGS= -Wall -O2
CXX_COMPILE=$(CXX) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS)
CXX_DEPENDENCIES=-Wp,-MD,.deps/$(*F).P
CXX_LINK=$(CXX) $(CFLAGS) $(LDFLAGS) -L.
VERSION=1.0
DISTDIR=$(top)pbzip2_custom-$(VERSION)
top_builddir = 

DEP_FILES=.deps/block.P .deps/block.P .deps/main.P .deps/radix.P .deps/mtf.P .deps/rle.P .deps/bwt.P .deps/radix.P .deps/bwt.P .deps/mtf.P .deps/rle.P

all: Debug

DEPS_MAGIC := $(shell mkdir .deps > /dev/null 2>&1 || :)
-include $(DEP_FILES)

%.o: %.cpp
	@echo '$(CXX_COMPILE) -c $<'; \
	$(CXX_COMPILE) $(CXX_DEPENDENCIES) -o $@ -c $<

Debug: $(Debug_OBJ)
	$(CXX_LINK) -o $@ $^

tags: 


clean:
	rm -f *.mod *.o *.obj .deps/*.P .lo

.PHONY: dist

dist:
	rm -rf $(DISTDIR)
	mkdir $(DISTDIR)
	cp $(Debug_SOURCES) $(ede_FILES) $(DISTDIR)
	tar -cvzf $(DISTDIR).tar.gz $(DISTDIR)
	rm -rf $(DISTDIR)

Makefile: Project.ede
	@echo Makefile is out of date!  It needs to be regenerated by EDE.
	@echo If you have not modified Project.ede, you can use 'touch' to update the Makefile time stamp.
	@false



# End of Makefile
