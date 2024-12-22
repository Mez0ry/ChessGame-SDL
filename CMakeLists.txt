cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(ProjectName Chess)

project(${ProjectName} LANGUAGES CXX VERSION 0.18.1)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CHESS_HEADERS 
    Chess/Application.hpp
)

set(CHESS_SOURCES 
    Chess/Application.cpp
    Chess/main.cpp
)

add_executable(${ProjectName} ${CHESS_HEADERS} ${CHESS_SOURCES})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/build/Stellar.log)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/build/Stellar.log DESTINATION "logs")
    file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/build/Stellar.log)
endif()


target_include_directories(${ProjectName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/Stellar/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/Stellar)
target_link_libraries(${ProjectName} fmt Stellar)