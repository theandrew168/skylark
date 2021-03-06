# skylark
This is a cross-platform [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) emulator written in C99.

## Dependencies
[SDL2](https://www.libsdl.org/index.php) (zlib) - Simple DirectMedia Layer

## Building
This project is built using POSIX-compatible [make](https://pubs.opengroup.org/onlinepubs/009695399/utilities/make.html).
For unix-like systems, it can be built natively.
For Windows builds, [mingw-w64](http://mingw-w64.org/doku.php) is used to cross-compile the project from a unix-like system.

### Linux (Native, Debian-based)
```
sudo apt install make libsdl2-dev
make
```

### macOS (Native)
```
brew install make sdl2
make -f Makefile.macos
```

### Windows (Cross-Compile)
From Linux (Debian-based):
```
sudo apt install make wget tar mingw-w64
make -f Makefile.mingw
```

From macOS:
```
brew install make wget gnu-tar mingw-w64
make -f Makefile.mingw
```

## References
[Emulator Tutorial](http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/)  
[CHIP-8 Specification](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)  
