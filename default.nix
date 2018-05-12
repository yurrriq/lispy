let
  fetchNixpkgs =
    { rev, sha256 }:
    builtins.fetchTarball {
      url = "https://github.com/NixOS/nixpkgs/archive/${rev}.tar.gz";
      inherit sha256;
    };

  importJSON = path: builtins.fromJSON (builtins.readFile path);
in

{ nixpkgs ? fetchNixpkgs (importJSON ./nixpkgs-src.json) }:

with import nixpkgs {
  config.packageOverrides = super: {
    mpc = super.callPackage ./pkgs/development/libraries/mpc {};
    xelatex-noweb = (super.texlive.combine {
      inherit (super.texlive) scheme-small
        beamer
        dirtytalk
        ec
        etoolbox
        fancyref
        fancyvrb
        float
        fontspec
        framed
        fvextra
        hardwrap
        ifplatform
        latexmk
        lineno
        mathtools
        minted
        realscripts
        setspace
        textcase
        titlesec
        todonotes
        tufte-latex
        upquote
        xetex
        xkeyval
        xltxtra
        xstring
        zapfding;
      });
  };
};

stdenv.mkDerivation rec {
  name = "lispy-${version}";
  version = builtins.readFile ./VERSION;
  src = ./src;

  outputs = [ "out" "docs" ];

  FONTCONFIG_FILE = makeFontsConf { fontDirectories = [ iosevka ]; };

  nativeBuildInputs = [
    gcc
    indent
    iosevka
    xelatex-noweb
    nix
    noweb
    python36Packages.pygments
    which
  ];
  buildInputs = [
    libedit.dev
    mpc.dev
  ];
}
