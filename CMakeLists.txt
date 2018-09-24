cmake_minimum_required(VERSION 3.12)
project(rz4m)

set(CMAKE_CXX_STANDARD 17)

link_libraries(stdc++fs)

add_executable(rz4m src/main.cpp src/Utils/Utils.cpp src/Utils/Utils.h src/Types/Types.cpp src/Types/Types.h src/Types/Types.cpp src/Engine/Ejector/Ejector.cpp src/Engine/Ejector/Ejector.h)