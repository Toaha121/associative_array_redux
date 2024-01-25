
##
## This file builds assignment 4
##

##
## Some macros (makefile 'variables') are defined by default
## by make(1).  The ones relevant to C compiling are:
##  CC     : the name of the compiler (defaults to system compiler)
##  CFLAGS : the flags (options) to pass to the compiler
##
## There are similar macros for other languages, for example C++ uses
##  CXX      : the name of the c++ compiler (defaults to system compiler)
##  CXXFLAGS : the flags (options) to pass to the c++ compiler


## explicitly add debugger support to each file compiled,
## and turn on all warnings.  If your compiler is surprised by your
## code, you should be too.
CFLAGS = -g -Wall -Iaalib -I.

## uncomment/change this next line if you need to use a non-default compiler
#CC = cc


##
## We can define variables for values we will use repeatedly below
##

## define the executables we want to build
A4_AA_EXE = a4aa
A4_TRIE_EXE = a4trie
A4_FASTA_EXE = a4fasta


## define the set of object files we need to build each executable
A4_COMMON_OBJS		= \
			keyprint.o \
			data-reader.o

A4_AA_OBJS		= \
			aa_mainline.o

A4_TRIE_OBJS		= \
			trie_mainline.o

A4_FASTA_OBJS		= \
			fasta_read.o \
			fasta_mainline.o

AALIB = libAAtrie.a

AALIBOBJS	= \
			aalib/aawrapper.o \
			aalib/trie-delete.o \
			aalib/trie-insert.o \
			aalib/trie-iterator.o \
			aalib/trie-query.o \
			aalib/trie.o

##
## TARGETS: below here we describe the target dependencies and rules
##
all: $(A4_AA_EXE) $(A4_TRIE_EXE) $(A4_FASTA_EXE)

$(A4_AA_EXE): $(AALIB) $(A4_AA_OBJS) $(A4_COMMON_OBJS)
	$(CC) $(CFLAGS) -L. -o $(A4_AA_EXE) $(A4_AA_OBJS) $(A4_COMMON_OBJS) -lAAtrie

$(A4_TRIE_EXE): $(AALIB) $(A4_TRIE_OBJS) $(A4_COMMON_OBJS)
	$(CC) $(CFLAGS) -L. -o $(A4_TRIE_EXE) $(A4_TRIE_OBJS) $(A4_COMMON_OBJS) -lAAtrie

$(A4_FASTA_EXE): $(AALIB) $(A4_FASTA_OBJS) $(A4_COMMON_OBJS)
	$(CC) $(CFLAGS) -L. -o $(A4_FASTA_EXE) $(A4_FASTA_OBJS) $(A4_COMMON_OBJS) -lAAtrie


## The ar(1) tool is used to create static libraries.  On Linux
## this is still the tool to use, however other platforms are
## moving to the newer libtool(1).  That tool would use a command
## line like the following
##
#	libtool -static -o $(AALIB) $(AALIBOBJS)
$(AALIB): $(AALIBOBJS)
	ar rcs $(AALIB) $(AALIBOBJS)
	

## convenience target to remove the results of a build
clean :
	- rm -f $(A4_AA_OBJS) $(A4_AA_EXE)
	- rm -f $(A4_TRIE_OBJS) $(A4_TRIE_EXE)
	- rm -f $(A4_FASTA_OBJS) $(A4_FASTA_EXE)
	- rm -f $(A4_COMMON_OBJS)
	- rm -f $(AALIBOBJS) $(AALIB)


## tags -- editor support for function definitions
tags : dummy
	ctags *.c aalib/*.c

## a "dummy" dependency forces its parent to run always
dummy :

