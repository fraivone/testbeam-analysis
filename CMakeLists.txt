
#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(unpacker)

find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

include_directories($ENV{UNPACKER_HOME}/include)

set(sources
    $ENV{UNPACKER_HOME}/src/TestBeamMapping.cc
    $ENV{UNPACKER_HOME}/src/Rechit2D.cc
    $ENV{UNPACKER_HOME}/src/Cluster.cc
    $ENV{UNPACKER_HOME}/src/Digi.cc)

set(header $ENV{UNPACKER_HOME}/include/*.h)

#----------------------------------------------------------------------------
# Locate sources and headers for the unpacker
# NB: headers are included so they will show up in IDEs
#
# file(GLOB headers $ENV{UNPACKER_HOME}/include/Cluster.h)
# file(GLOB headers $ENV{UNPACKER_HOME}/include/Digi.h)

# file(GLOB headers $ENV{UNPACKER_HOME}/include/Event.h)
# file(GLOB headers $ENV{UNPACKER_HOME}/include/GEMAMCEventFormat.h)
# file(GLOB sources $ENV{UNPACKER_HOME}/src/TestBeamMapping.cc)
# file(GLOB sources $ENV{UNPACKER_HOME}/src/Cluster.cc)
# file(GLOB sources $ENV{UNPACKER_HOME}/src/Digi.cc)

#----------------------------------------------------------------------------
# Add the executable
#
add_executable(RawToDigi unpacker.cc ${sources} ${headers})
target_link_libraries(RawToDigi ${ROOT_LIBRARIES})

add_executable(DigiToRechits DigiToRechits.cc ${sources} ${headers})
target_link_libraries(DigiToRechits ${ROOT_LIBRARIES})

add_executable(Tracks Tracks.cc ${sources} ${headers})
target_link_libraries(Tracks ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Copy mapping files
#
configure_file(mapping/tracker_mapping.csv mapping/tracker_mapping.csv COPYONLY)
configure_file(mapping/ge21_mapping.csv mapping/ge21_mapping.csv COPYONLY)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS RawToDigi DESTINATION bin)
install(TARGETS DigiToRechits DESTINATION bin)
install(TARGETS Tracks DESTINATION bin)
