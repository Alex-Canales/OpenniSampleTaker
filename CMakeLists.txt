cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(SampleTaker)

# set(CMAKE_VERBOSE_MAKEFILE true)

find_package(SDL 1.2 REQUIRED)
include_directories(${SDL_INCLUDE_DIRS})
link_directories(${SDL_LIBRARY_DIRS})
add_definitions(${SDL_DEFINITIONS})

set(OPENNI2_DEFINITIONS ${PC_OPENNI_CFLAGS_OTHER})
find_library(OPENNI2_LIBRARY
             NAMES OpenNI2
             HINTS ${PC_OPENNI2_LIBDIR} ${PC_OPENNI2_LIBRARY_DIRS} /usr/lib
             PATHS "$ENV{PROGRAMFILES}/OpenNI2/Lib${OPENNI2_SUFFIX}" "$ENV{PROGRAMW6432}/OpenNI2/Lib${OPENNI2_SUFFIX}" "$ENV{PROGRAMW6432}/OpenNI2"
             PATH_SUFFIXES lib lib64
)
find_path(OPENNI2_INCLUDE_DIR OpenNI.h
          HINTS ${PC_OPENNI2_INCLUDEDIR} ${PC_OPENNI2_INCLUDE_DIRS}
                  /usr/include/openni2 /usr/include/ni2
          PATHS "$ENV{PROGRAMFILES}/OpenNI2/include" "$ENV{PROGRAMW6432}/OpenNI2/include"
          PATH_SUFFIXES openni2 ni2
)

add_executable(sampleTaker main.cpp sampleTaker.cpp displayer.cpp)
target_link_libraries(sampleTaker ${SDL_LIBRARY} ${OPENNI2_LIBRARY})
