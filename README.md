# Untitled Game
## WIP 

### TODO
 - [x] Works?
 - [ ] Code cleanup
   - [ ] Abstract Shadow Volume creation
   - [ ] Remove abuse of std::vector::push_back's

## Compiling
Install `Cmake`, `SDL2`, `glm` and `glew` with your package manager.

Then create a directory for the build files.

  $ mkdir build

Then generate the build:

  $ cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Release

and build:

  $ cmake --build ./build

To run:

  $ ./build/src/game

