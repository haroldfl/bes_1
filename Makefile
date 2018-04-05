##
## @file Makefile
## Betriebssysteme Myfind Makefile
## Beispiel 1
##
## @author ibrahim Milli <ic17b063@technikum-wien.at>
## @author Dominic Ernstbrunner <ic17b015@technikum-wien.at>
## @author Florian Harold <ic17b093@technikum-wien.at>



CC=gcc52
CFLAGS=-DDEBUG -Wall -pedantic -Werror -Wextra -Wstrict-prototypes -Wformat=2 -fno-common -ftrapv -g -O3 -std=gnu11
CP=cp
CD=cd
MV=mv
GREP=grep
DOXYGEN=doxygen

OBJECTS=myfind.o

EXCLUDE_PATTERN=footrulewidth


## rules --


%.o: %.c
	$(CC) $(CFLAGS) -c $<


## targets --



all: myfind

hello: $(OBJECTS)
	$(CC) $(CFLAGS) -o "$@" "$^"

.PRECIOUS: %.tex

.PHONY: clean
clean:
	$(RM) *.o *~ myfind


distclean: clean
	$(RM) -r doc

doc: html pdf


html:
	$(DOXYGEN) doxygen.dcf

pdf: html
	$(CD) doc/pdf && \
	$(MV) refman.tex refman_save.tex && \
	$(GREP) -v $(EXCLUDE_PATTERN) refman_save.tex > refman.tex && \
	$(RM) refman_save.tex && \
	make && \
	$(MV) refman.pdf refman.save && \
	$(RM) *.pdf *.html *.tex *.aux *.sty *.log *.eps *.out *.ind *.idx \
	      *.ilg *.toc *.tps Makefile && \
	$(MV) refman.save refman.pdf


