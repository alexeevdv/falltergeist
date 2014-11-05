Falltergeist
============

Opensource crossplatform Fallout 2™ engine writen in C++ and SDL.
https://falltergeist.org/

Current version: 0.1.6

IRC channel: #falltergeist on [freenode.net](http://webchat.freenode.net/?channels=falltergeist)


##Compilation under linux

```
cmake . && make
```

## Running under linux

Put master.dat and critter.dat files into the falltergeist data directory, that is

* `~/.local/share/falltergeist/` on Linux,
*  `~/Library/Application Support/falltergeist` on OS X,
* `%APPDATA%/falltergeist` on Windows,

or mount CD-ROM with original game, then run

```
./falltergeist
```
