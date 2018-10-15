set(CMAKE_CXX_STANDARD 17)

set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)

find_package(Boost 1.40 COMPONENTS program_options filesystem REQUIRED)

include_directories(
    ${Boost_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(
        rz4
        main.cpp
        Utils/Utils.cpp
        Types/Types.cpp
        Engine/Scanner.cpp
        Engine/Compressor.cpp
        Engine/Formats/RiffWave.cpp
)

target_link_libraries(rz4 LINK_PUBLIC ${Boost_LIBRARIES})