nix-shell = nix-shell --pure


.PHONY: all
all: bin docs srcs


.PHONY: bin
bin: srcs
ifeq (,${IN_NIX_SHELL})
	@ ${nix-shell} --run 'nix-build'
else
	@ nix-build
endif
	@ find -L result -executable -type f -exec install -vm755 -Dt $@ {} +
	@ ${RM} result


.PHONY: srcs
srcs:
ifeq (,${IN_NIX_SHELL})
	@ ${nix-shell} --run 'make -C src $@'
else
	@ ${MAKE} -C src $@
endif


.PHONY: docs
docs:
ifeq (,${IN_NIX_SHELL})
	@ ${nix-shell} --run 'nix-build -A $@'
else
	@ nix-build -A $@
endif
	@ find -L result-$@ -name '*.pdf' -type f -exec install -vm644 -Dt $@ {} +
	@ ${RM} result-$@
