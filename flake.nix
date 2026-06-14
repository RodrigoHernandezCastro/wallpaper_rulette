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
      packages = forEachSupportedSystem (
        { pkgs, system }:
        {
          default = pkgs.stdenv.mkDerivation {
            pname = "wallpaper_rulette";
            version = "0.2.0";
            src = ./.;
            nativeBuildInputs = with pkgs; [
              cmake
              pkg-config
              wrapGAppsHook
            ];
            buildInputs = with pkgs; [
              gtk3
              libayatana-appindicator
              nlohmann_json
            ];
          };
        }
      );

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
                    gtk3
                    libayatana-appindicator
                    gsettings-desktop-schemas
                    self.formatter.${system}
                  ]
                  ++ pkgs.lib.optionals (!stdenv.hostPlatform.isDarwin) [ gdb ];
                buildInputs = with pkgs; [
                  nlohmann_json
                ];
                shellHook = ''
                  for d in \
                    ${pkgs.gsettings-desktop-schemas}/share/gsettings-schemas/gsettings-desktop-schemas-${pkgs.gsettings-desktop-schemas.version} \
                    ${pkgs.gtk3}/share/gsettings-schemas/gtk+3-${pkgs.gtk3.version}
                  do
                    if [ -d "$d" ]; then
                      export XDG_DATA_DIRS="$d''${XDG_DATA_DIRS:+:}$XDG_DATA_DIRS"
                    fi
                  done
                  echo "GSettings schemas registered"
                '';
              };
        }
      );

      formatter = forEachSupportedSystem ({ pkgs, ... }: pkgs.nixfmt);
    };
}
