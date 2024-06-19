{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/release-24.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          config.allowUnfree = true;
        };

        powerlifted = pkgs.callPackage ./powerlifted.nix { };
      in
      {
        packages = {
          inherit powerlifted;
          default = powerlifted;
        };
      });
}
