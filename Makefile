CFLAGS ?= -Wall -std=c99
cpif   ?= | cpif


ifneq (,$(findstring B,$(MAKEFLAGS)))
latexmk_flags = -gg
endif

latexmk_flags += -cd -pdf


.SUFFIXES: .nw .c .tex .pdf


.PHONY: all
all: src/prompt.c bin/prompt docs/prompt.pdf


.nw.c:
	notangle -R$(notdir $@) $< ${cpif} $@
	indent -kr -nut $@


.tex.pdf:
	ln -sf ../src/preamble.tex docs/
	latexmk ${latexmk_flags} $<
	rm docs/preamble.tex


bin/%: src/%.c
	@ mkdir -p $(dir $@)
	${CC} ${CFLAGS} -o $@ $<


docs/%.tex: src/%.nw
	noweave -n -delay -index $< ${cpif} $@
