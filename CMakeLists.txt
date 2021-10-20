# CMakeList.txt : CMake project for chess_cmake, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:80000000")

project ("chess_cmake")

# Add source to this project's executable.
add_executable (chess_cmake "chess_cmake.cpp" "chess_cmake.h"  "bitboard.h" "commons.h" "debug_utils.h" "board.h" "move.h" "board.cpp"  "engine.h" "eval.h" "eval_constants.h" "bench.h")

# TODO: Add tests and install targets if needed.
