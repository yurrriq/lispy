{ stdenv, fetchFromGitHub, fetchpatch }:

stdenv.mkDerivation rec {
  name = "mpc-${version}";
  version = "89eb7321";

  src = fetchFromGitHub {
    owner = "orangeduck";
    repo = "mpc";
    rev = version;
    sha256 = "037ff165zy32g77pwh9zh0fb7s7706zidy1n5dki9vnvx59lxzfm";
  };

  patches = [
    (fetchpatch {
      name = "fix-minor-memory-link.patch";
      url = https://patch-diff.githubusercontent.com/raw/orangeduck/mpc/pull/82.patch;
      sha256 = "0ilraklz6m49ryinxrz4j8rhc3hjnykir2v1j8bdgzq2c6kmdx4a";
    })
  ];

  doCheck = true;

  buildPhase = ''
    gcc -c -fPIC mpc.c -lm -o mpc.o
    gcc -shared mpc.o -o libmpc.so
  '';

  outputs = [ "out" "dev" ];

  installPhase = ''
    install -m644 -Dt "$dev/include" mpc.h
    install -m755 -Dt "$out/lib" libmpc.so
  '';

  meta = with stdenv.lib; {
    inherit (src) homepage;
    description = "A Parser Combinator library for C";
    license = licenses.bsd2;
    platforms = platforms.all;
  };
}
