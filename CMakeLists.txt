
#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(unpacker)

find_package(ROOT REQUIRED Minuit2)
include(${ROOT_USE_FILE})

include_directories($ENV{ANALYSIS_HOME}/include)

set(sources
    $ENV{ANALYSIS_HOME}/src/StripMapping.cc
    $ENV{ANALYSIS_HOME}/src/ChamberMapping.cc
    $ENV{ANALYSIS_HOME}/src/DataFrame.cc
    $ENV{ANALYSIS_HOME}/src/Track2D.cc
    $ENV{ANALYSIS_HOME}/src/Track.cc
    $ENV{ANALYSIS_HOME}/src/LinearFcn.cc
    $ENV{ANALYSIS_HOME}/src/DetectorTracker.cc
    $ENV{ANALYSIS_HOME}/src/DetectorLarge.cc
    $ENV{ANALYSIS_HOME}/src/Hit.cc
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

add_executable(DigiToRechits DigiToRechits.cc ${sources} ${headers})
target_link_libraries(DigiToRechits ${ROOT_LIBRARIES})

add_executable(Tracking Tracking.cc ${sources} ${headers})
target_link_libraries(Tracking ${ROOT_LIBRARIES})

add_executable(Alignment Alignment.cc ${sources} ${headers})
target_link_libraries(Alignment ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Copy script and analysis files
#
configure_file(mapping/oct2021/tracker_mapping.csv mapping/oct2021/tracker_mapping.csv COPYONLY)
configure_file(mapping/oct2021/ge21_mapping.csv mapping/oct2021/ge21_mapping.csv COPYONLY)
configure_file(mapping/oct2021/chamber_mapping.csv mapping/oct2021/chamber_mapping.csv COPYONLY)
configure_file(mapping/nov2021/me0_mapping.csv mapping/oct2021/me0_mapping.csv COPYONLY)
configure_file(mapping/nov2021/20x10_mapping.csv mapping/oct2021/20x10_mapping.csv COPYONLY)

configure_file(mapping/nov2021/tracker_mapping.csv mapping/nov2021/tracker_mapping.csv COPYONLY)
configure_file(mapping/nov2021/ge21_mapping.csv mapping/nov2021/ge21_mapping.csv COPYONLY)
configure_file(mapping/nov2021/me0_mapping.csv mapping/nov2021/me0_mapping.csv COPYONLY)
configure_file(mapping/nov2021/20x10_mapping.csv mapping/nov2021/20x10_mapping.csv COPYONLY)
configure_file(mapping/nov2021/chamber_mapping.csv mapping/nov2021/chamber_mapping.csv COPYONLY)

configure_file(analysis/alignment.py analysis/alignment.py COPYONLY)
configure_file(analysis/efficiency.py analysis/efficiency.py COPYONLY)
configure_file(analysis/residuals.py analysis/residuals.py COPYONLY)
configure_file(analysis/profiles.py analysis/profiles.py COPYONLY)
configure_file(analysis/ge21.py analysis/ge21.py COPYONLY)
configure_file(analysis/strips.py analysis/strips.py COPYONLY)
configure_file(analysis/horns.py analysis/horns.py COPYONLY)
configure_file(analysis/occupancy.py analysis/occupancy.py COPYONLY)
configure_file(analysis/scan.py analysis/scan.py COPYONLY)
configure_file(analysis/track_angles.py analysis/track_angles.py COPYONLY)

configure_file(scripts/angles.sh scripts/angles.sh COPYONLY)
configure_file(scripts/scan.sh scripts/scan.sh COPYONLY)
configure_file(scripts/angles.py scripts/angles.py COPYONLY)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS RawToDigi DESTINATION bin)
install(TARGETS DigiToRechits DESTINATION bin)
install(TARGETS Tracking DESTINATION bin)
