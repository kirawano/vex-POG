{
  description = "vex 85711C: Pot of Greed";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }: {
    system = "x86_64-linux";

  };
}
