# Getting started

**Note**: You need to install premake5 to generate the build system for Windows and Linux

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

# Gameplay

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