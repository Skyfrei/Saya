{
  description = "Saya sdl and wayland compatible";

    inputs = {
        nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    };

    outputs = { self, nixpkgs, ... }:
        let
            system = "x86_64-linux";
            pkgs = nixpkgs.legacyPackages.${system};
        in{
        packages.${system}.default = pkgs.stdenv.mkDerivation {
            pname = "Saya";
            version = "0.9";
            src = self;
            buildInputs = [
                    pkgs.perl
                    pkgs.pkg-config
                    pkgs.sdl3
                    pkgs.sdl3-ttf
                    pkgs.catch2_3
                    pkgs.cmake
                    pkgs.libtorch-bin
                    pkgs.ninja
                    pkgs.binutils
            ];
            configurePhase = "
                cmake -B build/ -G Ninja .
            ";
            
            buildPhase = "
                ninja -C build/
            ";
            installPhase = "
                mkdir -p $out/
                mv build/Saya $out/
                mv build/tester $out/
            ";
        };
        apps.${system} = {
            default = {
                type = "app";
                program = "${self.packages.${system}.default}/Saya";
                meta.description = "The full Saya app";
            };
            tester = {
                type = "app";
                program = "${self.packages.${system}.default}/tester";
                meta.description = "Testing specific use cases";
            };
        };

        bundlers.${system}.default = rec {
            identity = drv: drv;
            saya = drv: self.packages.${system}.default;
            default = identity;
        };

        devShells.${system}.Saya = pkgs.stdenv.mkDerivation {
            pname = "Saya";
            version = "0.9";
            buildInputs = [
                pkgs.perl
                pkgs.pkg-config
                pkgs.sdl3
                pkgs.sdl3-ttf
                pkgs.catch2_3
                pkgs.cmake
                pkgs.libtorch-bin
                pkgs.ninja
                pkgs.latexmk
                pkgs.texlive-env
            ];

        };
    };

}

