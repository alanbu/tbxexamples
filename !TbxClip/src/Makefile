# Makefile for TBX clipboard example

# If you have fix deps uncomment following line for dependency scanning
include FixDeps:Rules/make

CXX=g++
CXXFLAGS = -O2 -Wall -mpoke-function-name -mthrowback -ITBX:

LD = g++
LDFLAGS = -LTBX: -ltbx -lstdc++ -static

CCSRC = $(wildcard *.cc)
OBJECTS = $(CCSRC:.cc=.o)

^.!RunImage:	!RunImageE1f
	elf2aif !RunImageE1f ^.!RunImage
!RunImageE1f:	$(OBJECTS)
	$(LD) $(LDFLAGS) -o !RunImageE1f $(OBJECTS)

clean:
	rm -f $(OBJECTS)

# IF you have fix deps uncomment following line
-include $(CCSRC:.cc=.d)
