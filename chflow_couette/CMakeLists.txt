cmake_minimum_required(VERSION 2.6)
project(chflow_couette)

# Add C++11 support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(LIBS_INCLUDE
    lib/include
    )

set(CHANNEL_FLOW_LIB
    ${CMAKE_SOURCE_DIR}/lib/libchflow.so
    )

#set(THEQUICK_LIBS
#    ${CMAKE_SOURCE_DIR}/lib/libthequick_core.so
#    ${CMAKE_SOURCE_DIR}/lib/libthequick_INIparser.so
#    )

add_subdirectory(src/thequick_light)

set(SOURCES
        src/couette.cpp
    )

set(OUTPUT_DIRECTORY bin CACHE STRING "")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${OUTPUT_DIRECTORY})

include_directories(${LIBS_INCLUDE})
add_executable(${PROJECT_NAME} ${SOURCES})
#target_link_libraries(${PROJECT_NAME} ${THEQUICK_LIBS} ${CHANNEL_FLOW_LIB})
target_link_libraries(${PROJECT_NAME} thequick_light ${CHANNEL_FLOW_LIB})

file(GLOB RESOURCES "res/*")
file(COPY ${RESOURCES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
