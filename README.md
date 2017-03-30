# skylark
This is a Chip8 emulator written in ANSI C that utilizes SDL2 for graphics.

## Building on Linux
First ensure that the SDL2 library is installed.  
On Ubuntu 16.04:
```bash
sudo apt install libsdl2-dev
```
Then, clone this repository to get the source code:
```bash
git clone https://github.com/theandrew168/skylark.git
```
Now, build the project using `make`.
```bash
make -j4    # or -jN, for whatever is appropriate for your system
```
Once built, skylark can be ran via:
```bash
./skylark <rom_file>    # where <rom_file> is the game you want to play
```

## Directories
`roms/` - Chip8 games used for playing / testing the emulator  
`src/` - Emulator source code  

## Libraries
[SDL2](https://www.libsdl.org/index.php) (zlib) - Window creation / basic rendering  

## References
[Emulator Tutorial](http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/)  
[Chip8 Specification](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)  

