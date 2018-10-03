set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.40 COMPONENTS program_options filesystem REQUIRED)

IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    link_directories(${Boost_LIBRARY_DIR})
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(
        rz4m
        main.cpp
        Utils/Utils.cpp
        Engine/Scanner/Scanner.cpp
        Engine/Ejector/Ejector.cpp
        ${PROJECT_SOURCES}
)

target_link_libraries(rz4m LINK_PUBLIC ${Boost_LIBRARIES})