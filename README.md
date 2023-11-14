# sukless2
A successor to the failed project sukless-chess, Sukless2 is a magic bitboard chess engine
written in C++. It aims to be a beautiful, readable, and (relatively) fast chess engine.

## Why?
There are many open source chess engines, some of them use magic bitboards, but none that I 
have seen are easily readable. Sukless2 aims to change that. Sukless2 isn't stockfish and 
will never be. The goal of this project (stated above) is to make an easily readable bitboard
chess engine written in a real mans language. Feast your eyes on real speed JavaScriptors!

## Whats different about Sukless2?
sukless2 vs. sukless-chess (sukless1) really boils down to speed. Expect a 25-50x speed
improvement over sukless1. Sukless2 will also have a far superior AI to sukless1.

## Future Plans
The hope is that sukless2 will eventually be seperated into client and server
applications

### Building sukless2
1. find a place that you would like to put vcpkg, i like ~/vcpkg
2. ```git clone git@github.com:microsoft/vcpkg.git```
3. ```cd vcpkg```
4. ```./vcpkg install sdl2 sdl2-image sdl2-mixer --triplet <your-triplet>```
5. ```./bootstrap_vcpkg```
6. ```./vcpkg integrate install```
7. if using cmake from the command line :
  from ~/sukless2 directory
  ```cmake -S . -B .build -DCMAKE_TOOLCHAIN_FILE=~/<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_ROOT=~/<path-to-vcpkg>;```
  ```cd <build-directory>; make```
  if using cmake tools extenstion in vscode:
  in settings.json under the .vscode folder in the repository:
  ```json 
    {
      "cmake.buildDirectory": "${workspaceFolder}/.build",
      "cmake.configureSettings": {
        "VCPKG_ROOT": "/<path-to-vcpkg>",
        "CMAKE_TOOLCHAIN_FILE": "<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake"
    },

    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"
    }
  ```
  ### Running Sukless2
  ```./<path-to-build-folder>/chess```
 