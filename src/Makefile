C_FLAGS ?= -std=c99 -Wall -Werror 
C_LIBS  := -ledit -lmpc
cpif    ?= | cpif


ifneq (,$(findstring B,$(MAKEFLAGS)))
latexmk_flags = -gg
endif

latexmk_flags += -cd -pdf -silent


.SUFFIXES: .nw .tex .pdf

.nw.tex:
	noweave -n -delay -index $< ${cpif} $@

.tex.pdf:
	latexmk ${latexmk_flags} $<


.PHONY: all srcs bins pdfs
all:  srcs bins pdfs
srcs: parsing.c
bins: parsing
pdfs: byol.pdf


parsing.c: byol.nw lispy.xxd
	notangle -R$@ $< ${cpif} $@
	indent -kr -nut $@


.INTERMEDIATE: lispy.mpc
lispy.mpc: byol.nw
	notangle -R$@ $< ${cpif} $@


lispy.xxd: lispy.mpc
	xxd -i <$< | sed '$$s/$$/, 0/' >$@	


parsing: parsing.c
	${CC} ${C_FLAGS}$< ${C_LIBS} -o $@


.PHONY: install
install:
	install -m755 -Dt "$$out/bin" parsing
	install -m644 -Dt "$$docs/" byol.pdf
