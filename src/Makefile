C_FLAGS ?= -std=c99 -Wall -Werror
C_LIBS  := -ledit -lm -lmpc
cpif    ?= | cpif


ifneq (,$(findstring B,$(MAKEFLAGS)))
latexmk_flags = -gg
endif

latexmk_flags += -pdf -silent


.SUFFIXES: .mpc .nw .tex .pdf .xxd

.mpc.xxd:
	xxd -i <$< >$@


.nw.tex:
	noweave -n -delay -index $< ${cpif} $@

.tex.pdf:
	latexmk ${latexmk_flags} $<


.PHONY: all srcs bins pdfs
all: lispy lispy.pdf


lispy.c: lispy.nw
	notangle -R$@ $< ${cpif} $@
	indent -kr -nut $@


lispy.mpc: lispy.nw
	notangle -R$@ $< ${cpif} $@


lispy: lispy.c lispy.xxd
	${CC} ${C_FLAGS} $< ${C_LIBS} -o $@


.PHONY: install
install: lispy lispy.pdf lispy.mpc lispy.c
	install -m755 -Dt "$$out/bin/" lispy
	install -m644 -Dt "$$docs/" lispy.pdf
	install -m644 -Dt "$$dev/src/" lispy.mpc lispy.c
