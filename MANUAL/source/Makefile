.PHONY: all clean

MAKEINFO = makeinfo
INPUT = gnuclad.texi
INFOFILE = ../gnuclad.info
HTMLDIR = ../html-data


all: $(INPUT)
	$(MAKEINFO) $(INPUT) --output $(INFOFILE)
	$(MAKEINFO) --html --output $(HTMLDIR) $(INPUT)

clean:
	rm $(INFOFILE)
	rm -rf $(HTMLDIR)
