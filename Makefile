branch ?= $(shell git symbolic-ref --short HEAD)
nix-shell = nix-shell --pure
nix-build = nix-build --argstr branch ${branch}


.PHONY: all
all: bin dev docs


.PHONY: bin
bin:
ifeq (,${IN_NIX_SHELL})
	@ ${nix-shell} --run '${nix-build}'
else
	@ ${nix-build}
endif
	@ find -L result -executable -type f -exec install -vm755 -Dt $@ {} +
	@ ${RM} result


.PHONY: dev
dev:
ifeq (,${IN_NIX_SHELL})
	@ ${nix-shell} --run '${nix-build} -A $@'
else
	@ ${nix-build} -A $@
endif
	@ find result-$@/src -type f -exec install -vm644 -Dt src {} +
	@ ${RM} result-$@


.PHONY: docs
docs:
ifeq (,${IN_NIX_SHELL})
	@ ${nix-shell} --run '${nix-build} -A $@'
else
	@ ${nix-build} -A $@
endif
	@ find -L result-$@ -name '*.pdf' -type f -exec install -vm644 -Dt $@ {} +
	@ ${RM} result-$@
