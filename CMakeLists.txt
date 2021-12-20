
#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(unpacker)

find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

include_directories($ENV{ANALYSIS_HOME}/include)

set(sources
    $ENV{ANALYSIS_HOME}/src/TestBeamMapping.cc
    $ENV{ANALYSIS_HOME}/src/Rechit2D.cc
    $ENV{ANALYSIS_HOME}/src/Rechit.cc
    $ENV{ANALYSIS_HOME}/src/Cluster.cc
    $ENV{ANALYSIS_HOME}/src/Digi.cc)

set(header $ENV{ANALYSIS_HOME}/include/*.h)

#----------------------------------------------------------------------------
# Locate sources and headers for the unpacker
# NB: headers are included so they will show up in IDEs
#
# file(GLOB headers $ENV{ANALYSIS_HOME}/include/Cluster.h)
# file(GLOB headers $ENV{ANALYSIS_HOME}/include/Digi.h)

# file(GLOB headers $ENV{ANALYSIS_HOME}/include/Event.h)
# file(GLOB headers $ENV{ANALYSIS_HOME}/include/GEMAMCEventFormat.h)
# file(GLOB sources $ENV{ANALYSIS_HOME}/src/TestBeamMapping.cc)
# file(GLOB sources $ENV{ANALYSIS_HOME}/src/Cluster.cc)
# file(GLOB sources $ENV{ANALYSIS_HOME}/src/Digi.cc)

#----------------------------------------------------------------------------
# Add the executable
#
add_executable(RawToDigi unpacker.cc ${sources} ${headers})
target_link_libraries(RawToDigi ${ROOT_LIBRARIES})

add_executable(Multiplicity multiplicity.cc ${sources} ${headers})
target_link_libraries(Multiplicity ${ROOT_LIBRARIES})

add_executable(DigiToRechits DigiToRechits.cc ${sources} ${headers})
target_link_libraries(DigiToRechits ${ROOT_LIBRARIES})

add_executable(Tracking Tracking.cc ${sources} ${headers})
target_link_libraries(Tracking ${ROOT_LIBRARIES})

add_executable(Alignment Alignment.cc ${sources} ${headers})
target_link_libraries(Alignment ${ROOT_LIBRARIES})

add_executable(Analysis Analysis.cc ${sources} ${headers})
target_link_libraries(Analysis ${ROOT_LIBRARIES})

add_executable(FastEfficiency FastEfficiency.cc ${sources} ${headers})
target_link_libraries(FastEfficiency ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Copy mapping files
#
configure_file(mapping/tracker_mapping.csv mapping/tracker_mapping.csv COPYONLY)
configure_file(mapping/ge21_mapping.csv mapping/ge21_mapping.csv COPYONLY)
configure_file(analysis/alignment.py scripts/alignment.py COPYONLY)
configure_file(analysis/efficiency.py scripts/efficiency.py COPYONLY)
configure_file(analysis/residuals.py scripts/residuals.py COPYONLY)
configure_file(analysis/horns.py scripts/horns.py COPYONLY)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS RawToDigi DESTINATION bin)
install(TARGETS DigiToRechits DESTINATION bin)
install(TARGETS Tracking DESTINATION bin)
install(TARGETS Analysis DESTINATION bin)