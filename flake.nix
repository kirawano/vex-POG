{
  description = "vex 85711C: Pot of Greed";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = {self, nixpkgs}:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
      {
        devShells.${system}.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            (python313.withPackages (python-pkgs: with python-pkgs; [
              pip
              pyyaml
              jinja2
            ]))
            python313
            figlet
            lolcat
            libgcc
            gcc-arm-embedded
            tldr
	    gnumake
          ];

          shellHook = ''
                   export VIRTUAL_ENV_DISABLE_PROMPT=0
                   export PS1="vex (\w)\$ "
                   if [ -d .venv ]; then
                      source .venv/bin/activate
                   fi
                   pip install pros-cli "click<8.2.0"
                   clear
                   echo "pot of GREED" | figlet | lolcat
                   alias dih="echo 'cargooner' | figlet | lolcat"
                   alias vex-init="python -m venv .venv && source .venv/bin/activate && pr"
                   alias pog="echo 'pot of GREED' | figlet | lolcat"
		               alias vsh="python vsh/vsh.py"
          '';
        };

      };
}
