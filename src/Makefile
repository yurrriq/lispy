CFLAGS  ?= -ledit -std=c99 -Wall -Werror
NW_SRCS := $(wildcard *.nw)
cpif    ?= | cpif


ifneq (,$(findstring B,$(MAKEFLAGS)))
latexmk_flags = -gg
endif

latexmk_flags += -cd -pdf -silent


.SUFFIXES: .nw .c .tex .pdf

.nw.c:
	notangle -R$(notdir $@) $< ${cpif} $@
	indent -kr -nut $@

.nw.tex:
	noweave -n -delay -index $< ${cpif} $@

.tex.pdf:
	latexmk ${latexmk_flags} $<


.PHONY: all srcs bins pdfs
all:  srcs bins pdfs
srcs: ${NW_SRCS:.nw=.c}
bins: ${NW_SRCS:.nw=}
pdfs: ${NW_SRCS:.nw=.pdf}


%: %.c
	${CC} ${CFLAGS} -o $@ $<


.PHONY: install
install:
	install -dm755 "$$out/bin"
	install -m755 -Dt "$$out/bin" ${NW_SRCS:.nw=}
	install -dm755 "$$docs/"
	install -m644 -Dt "$$docs/" ${NW_SRCS:.nw=.pdf}
