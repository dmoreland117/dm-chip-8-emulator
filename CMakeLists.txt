cmake_minimum_required(VERSION 3.0.0)
project(Chip-8_Emulator VERSION 0.1.0 LANGUAGES C CXX)

#enable_testing()

add_executable(Chip-8_Emulator src/main.cpp src/dm_chip-8.cpp)
target_include_directories(Chip-8_Emulator PRIVATE include)

target_link_libraries(Chip-8_Emulator SDL2main SDL2)