{
  description = "devenv";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        devShells.default = pkgs.mkShell {
          name = "devenv";

          packages = with pkgs; [
            raylib
            cmake
          ];

          shellHook = ''
            exec $(getent passwd "$USER" | cut -d: -f7)
          '';
        };
        defaultPackage = pkgs.stdenv.mkDerivation {
          name = "stpartant";
          src = ./.;
          configurePhase = "cmake -B build -DCMAKE_BUILD_TYPE=Release";
          buildPhase = "cmake --build build";
          installPhase = "cmake --install build --prefix $out";

          nativeBuildInputs = with pkgs; [
            cmake
          ];

          buildInputs = with pkgs; [
            raylib
          ];
        };
      }
    );
}
