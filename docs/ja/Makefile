PANDOC = pandoc
PANDOCFLAGS = --self-contained -t html5 -c $(HOME)/.pandoc/github.css

all:index.html qmc-example.html mc-example.html oscillatory.html qmc_largesobol_example.html

%.html:%.md
	$(PANDOC) $(PANDOCFLAGS) -o $@ $<
