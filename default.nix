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

with import nixpkgs {};

stdenv.mkDerivation rec {
  name = "lispy-${version}";
  version = builtins.readFile ./VERSION;
  src = ./src;

  outputs = [ "out" "docs" ];
  nativeBuildInputs = [
    gcc
    indent
    iosevka
    glibc.static
    libedit.dev
    nix
    noweb
    python36Packages.pygments
    (texlive.combine {
      inherit (texlive) scheme-full tufte-latex;
    })
    which
  ];
}
