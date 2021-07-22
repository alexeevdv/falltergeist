Falltergeist [![Build Status](https://travis-ci.org/falltergeist/falltergeist.svg?branch=develop)](https://travis-ci.org/falltergeist/falltergeist) [![Version](https://img.shields.io/github/release/falltergeist/falltergeist.svg)](https://github.com/falltergeist/falltergeist/releases/latest) [![Discord](https://img.shields.io/discord/401990446747877376.svg)](https://discord.gg/jxs6WRq)
============

Opensource crossplatform Fallout 2™ engine writen in C++ and SDL.
https://falltergeist.org/

## Dependencies

- [SDL2](http://www.libsdl.org)
- [SDL2\_mixer](http://www.libsdl.org/projects/SDL_mixer/)
- [SDL2\_image](http://www.libsdl.org/projects/SDL_image/)
- [GLEW](http://glew.sourceforge.net/)
- [GLM](http://glm.g-truc.net/)
- [Zlib](http://www.zlib.net/)

## Compilation

### Linux

```console
cmake . && make
```

### Linux (Debian/Ubuntu)
```console
sudo apt-get install  build-essential libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libglew-dev libglm-dev zlib1g-dev

cmake . && make
```

### Windows (Visual Studio x64)

You could add dependencies manually but it is easier to use [Conan](https://docs.conan.io/en/latest/howtos/vs2017_cmake.html). 

First, set up your [Conan profile](https://docs.conan.io/en/latest/reference/profiles.html) to match your compiler or let conan autodetect your settings (`conan profile new vs --detect` and use ` --profile=vs` when running `conan install`). Visual Studio 2019 profile should look like this:

```ini
[settings]
os=Windows
os_build=Windows
arch=x86_64
arch_build=x86_64
compiler=Visual Studio
compiler.version=16
build_type=Debug
```
If using VS2017 or building on Release then use: `compiler.version=15`, or `build_type=Release` respectively.

To be able to download conan SDL2 packages, further configure your conan instance by introducing a new remote:
```cmd
conan remote add bincrafters https://bincrafters.jfrog.io/artifactory/api/conan/public-conan
```

Then open 'Developer PowerShell for Visual Studio' and execute the following commands:

```cmd
mkdir build
cd build
conan install .. --build missing
cmake .. -G "Visual Studio 16 2019" -A x64
devenv /build Debug falltergeist.sln
```
Or open and build the solution in Visual Studio instead of running the devenv command.
If using VS2017 or building on Release then use: `“Visual Studio 15 2017 Win64”`, or `devenv /build Debug` respectively.


## Running

Put **master.dat** and **critter.dat** files into the falltergeist data directory, that is

* `~/.local/share/falltergeist/`  or `/usr/local/share/falltergeist` (for global installs) on Linux,
* `~/Library/Application Support/falltergeist` on OS X,
* `%APPDATA%/falltergeist` on Windows,

or mount CD-ROM with original game, then run `./falltergeist` on \*nix or `.\bin\falltergeist.exe` on Windows.

## Development roadmap

This is a pretty highlevel roadmap to get to a playable game. The idea behind it is to step by step get closer to the full game.

- Worldmap support
- Elevators
- NPCs
- Combat AI
- Full scripting support
- Skills
