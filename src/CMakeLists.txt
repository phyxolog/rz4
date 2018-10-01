set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

link_libraries(stdc++fs)
link_directories(${Boost_LIBRARY_DIR})

add_executable(rz4m main.cpp Utils/Utils.cpp ${PROJECT_SOURCES})

target_link_libraries(rz4m LINK_PUBLIC ${Boost_LIBRARIES})