let
  fetchNixpkgs =
    { rev, sha256 }:
    builtins.fetchTarball {
      url = "https://github.com/NixOS/nixpkgs/archive/${rev}.tar.gz";
      inherit sha256;
    };

  importJSON = path: builtins.fromJSON (builtins.readFile path);
in

{ nixpkgs ? fetchNixpkgs (importJSON ./nixpkgs-src.json)
, branch ? "develop"
}:

with import nixpkgs {
  config.packageOverrides = super: {
    mpc = super.callPackage ./pkgs/development/libraries/mpc {};
    xelatex-noweb = (super.texlive.combine {
      inherit (super.texlive) scheme-small
        beamer
        datatool
        dirtytalk
        ec
        etoolbox
        fancyref
        fancyvrb
        float
        fontspec
        framed
        fvextra
        glossaries
        hardwrap
        ifplatform
        latexmk
        lineno
        mathtools
        mfirstuc
        minted
        realscripts
        setspace
        substr
        textcase
        titlesec
        tocbibind
        todonotes
        tufte-latex
        upquote
        xetex
        xindy
        xfor
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

  preBuild = ''
    substituteInPlace lispy.nw --replace '%VERSION%' '${version}'
    substituteInPlace preamble.tex --replace '%BRANCH%' '${branch}'
  '';

  outputs = [ "out" "docs" "dev" ];

  FONTCONFIG_FILE = makeFontsConf { fontDirectories = [ iosevka ]; };

  nativeBuildInputs = [
    gcc
    gdb
    indent
    iosevka
    xelatex-noweb
    nix
    noweb
    python36Packages.pygments
    which
    xxd
  ];

  buildInputs = [
    libedit.dev
    mpc.dev
  ];
}
