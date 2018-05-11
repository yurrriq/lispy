nix-shell ?= nix-shell --pure


.PHONY: all
all: bin docs srcs


.PHONY: bin
bin:
	@ ${nix-shell} --run 'nix-build'
	@ find -L result -executable -type f -exec install -vm755 -Dt $@ {} +
	@ ${RM} result


.PHONY: srcs
srcs:
	${nix-shell} --run 'make -C src $@'



.PHONY: docs
docs:
	@ ${nix-shell} --run 'nix-build -A $@'
	@ find -L result-$@ -name '*.pdf' -type f -exec install -vm644 -Dt $@ {} +
	@ ${RM} result-$@
