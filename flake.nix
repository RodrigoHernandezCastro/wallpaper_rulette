{
  description = "A Nix-flake-based C/C++ development environment";

  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";

  outputs =
    { self, ... }@inputs:

    let
      supportedSystems = [
        "x86_64-linux"
        "aarch64-linux"
        "aarch64-darwin"
      ];
      forEachSupportedSystem =
        f:
        inputs.nixpkgs.lib.genAttrs supportedSystems (
          system:
          f {
            inherit system;
            pkgs = import inputs.nixpkgs { inherit system; };
          }
        );
    in
    {
      packages = forEachSupportedSystem ({ pkgs, system }: {
        default = pkgs.stdenv.mkDerivation {
          pname = "wallpaper_rulette";
          version = "0.1.0";
          src = ./.;
          nativeBuildInputs = with pkgs; [ cmake ];
          buildInputs = with pkgs; [ nlohmann_json ];
        };
      });

      devShells = forEachSupportedSystem (
        { pkgs, system }:
        {
          default =
            pkgs.mkShell.override
              {
                # Override stdenv in order to change compiler:
                # stdenv = pkgs.clangStdenv;
              }
              {
                packages =
                  with pkgs;
                  [
                    clang-tools
                    cmake
                    pkg-config
                    self.formatter.${system}
                  ]
                  ++ pkgs.lib.optionals (!stdenv.hostPlatform.isDarwin) [ gdb ];
                buildInputs = with pkgs; [
                  nlohmann_json
                ];
              };
        }
      );

      formatter = forEachSupportedSystem ({ pkgs, ... }: pkgs.nixfmt);
    };
}
