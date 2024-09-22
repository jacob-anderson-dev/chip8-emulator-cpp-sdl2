# A simple Chip 8 emulator made with C++ and SDL2.
followed along with:
https://austinmorlan.com/posts/chip8_emulator/

was originally planning on doing this one with Ada 95, but the gtkada and qt ada bindings don't like to play nice with arm64 on the M1 Macs.
Will be uploading the sister project when I get on to my Windows machine.

build by opening a terminal at the root directory of the repository and running:
```
mkdir build
cd build
cmake ..
make
```

Usage to run the program:
```
./bin/MyProject <SCALE> <DELAY> <ROM>
```

where 
  ```<SCALE>``` refers to what multiple you want to scale up the Chip 8 64 x 32 screen,
  ```<DELAY>``` refers to how fast the clock should go (16 is about 60fps, 1 is really fast, etc.),
  and ```<ROM>``` refers to the path to a Chip 8 rom to run.
