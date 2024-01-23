## Specification

- Turn based
- Map made out of connected tiles
- A tile consists of:
    - Units
    - Buildings
    - Allegiance
    - Income value
- Tiles can belong to a player or be neutral
- Occupied tiles provide their income to their players
- Units can be moved from a tile to any adjacent tile
- If units are moved to a non-friendly tile with units a battle will occur, if the attacking player wins the tile is occupied by that player
- Recruiting certain units requires the presence of specific buildings on the tile where they are being recruited.

## Win condition:

- The game is over when only one player occupies tiles on the map

# Getting started

**Note**: You need to install [premake5](https://premake.github.io/) to generate the build system for Windows and Linux

## Clone the repository
```shell
git clone --recursive https://github.com/wikcioo/ultimate-war.git
cd ultimate-war
```

## Windows

Generate the Visual Studio solution
```shell
premake5 vs2022
```

Open the Visual Studio solution and click the run button

## Linux

Generate the Makefile build system
```shell
premake5 gmake
```

Compile and run the project
```
make -j && ./bin/Debug-linux/UltimateWar
```

# Screenshots

![Main Menu UI screenshot](docs/screenshots/main-menu-ui-milestone5.png?raw=true)
![Choose Players UI screenshot](docs/screenshots/choose-players-ui-milestone5.png?raw=true)
![Gameplay UI screenshot](docs/screenshots/game-ui-milestone5.png?raw=true)
![Map Editor UI screenshot](docs/screenshots/map-editor-ui-milestone5.png?raw=true)
