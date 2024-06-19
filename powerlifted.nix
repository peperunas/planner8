{ stdenv
, cmake
, python311
, osi
, boost
, pypy3
}:

stdenv.mkDerivation {
  pname = "powerlifted";
  version = "ipc2023";

  src = ./.;

  nativeBuildInputs = [ cmake python311.pkgs.wrapPython boost ];
  buildInputs = [ python311 osi pypy3 ];
  NIX_CFLAGS_COMPILE = "-Wno-deprecated-declarations -Wno-deprecated-builtins";
  configurePhase = ''
    pushd ext/cpddl
      patchShebangs .
      make && make -C bin
    popd

    pushd ext/powerlifted
      python build.py
    popd
  '';

  installPhase = ''
    mkdir -p $out/bin
    
    pushd ext/powerlifted
      cp -r {build.py,builds,driver,src} $out/bin
      cp -r powerlifted.py $out/bin/powerlifted
    popd

    cp -r ext/cpddl/bin/{pddl,pddl-fdr,pddl-lplan,pddl-pddl,pddl-symba} $out/bin
    
    wrapProgram "$out/bin/powerlifted" --set CPDDL_BIN $out/bin
  '';
}
