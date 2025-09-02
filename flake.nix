{
  description = "SDL + Wayland minimal dev shell";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in {
      devShells.${system}.default = pkgs.mkShell {

        buildInputs = [
          pkgs.pkg-config
          pkgs.sdl3
          pkgs.sdl3-ttf
          pkgs.catch2_3

        ];

        };
    };
}

