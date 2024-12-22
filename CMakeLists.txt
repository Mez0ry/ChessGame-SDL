cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_is_supported OUTPUT error)

set(ProjectName Chess)

project(${ProjectName} LANGUAGES CXX VERSION 0.18.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)


if(MSVC)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)
set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")  
endif()

file(GLOB_RECURSE PROJECT_SOURCE_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${ProjectName}/*.cpp")

add_executable(${ProjectName} ${PROJECT_SOURCE_FILES})

if( ipo_is_supported )
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET ${ProjectName} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/build/Stellar.log)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/build/Stellar.log DESTINATION "logs")
    file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/build/Stellar.log)
endif()

target_include_directories(${ProjectName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/Stellar/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/Stellar)
target_link_libraries(${ProjectName} fmt Stellar)