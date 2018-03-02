cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(monkeys-audio)

set(CMAKE_CXX_STANDARD 11)

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -pedantic -Wno-long-long")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} -O0")
endif(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

include(CheckFunctionExists)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(TestBigEndian)

check_function_exists(memcmp HAVE_MEMCMP)
check_function_exists(stat HAVE_STAT)
check_function_exists(wcscasecmp HAVE_WCSCASECMP)

check_include_files(sys/ioctl.h HAVE_SYS_IOCTL_H)
check_include_files(sys/time.h HAVE_SYS_TIME_H)
check_include_files(fcntl.h HAVE_FCNTL_H)
check_include_files(float.h HAVE_FLOAT_H)
check_include_files(memory.h HAVE_MEMORY_H)
check_include_files(stdlib.h HAVE_STDLIB_H)
check_include_files(string.h HAVE_STRING_H)
check_include_files(stdarg.h HAVE_STDARG_H)
check_include_files(unistd.h HAVE_UNISTD_H)

if(HAVE_STDLIB_H AND HAVE_STDARG_H AND HAVE_STRING_H AND HAVE_FLOAT_H)
    set(STDC_HEADERS TRUE)
endif(HAVE_STDLIB_H AND HAVE_STDARG_H AND HAVE_STRING_H AND HAVE_FLOAT_H)

check_library_exists(rt nanosleep "" NANOSLEEP_IN_RT)
if(NANOSLEEP_IN_RT)
    set(NANOSLEEP_LIBRARY rt)
else(NANOSLEEP_IN_RT)
    check_library_exists(posix4 nanosleep "" NANOSLEEP_IN_POSIX4)
    if(NANOSLEEP_IN_POSIX4)
        set(NANOSLEEP_LIBRARY posix4)
    endif(NANOSLEEP_IN_POSIX4)
endif(NANOSLEEP_IN_RT)

if(NOT WIN32)
    set(BUILD_CROSS_PLATFORM TRUE)
endif(NOT WIN32)

test_big_endian(WORDS_BIGENDIAN)

configure_file(
    ${PROJECT_SOURCE_DIR}/Source/Shared/config.h.in
    ${PROJECT_BINARY_DIR}/config.h)

set(SHARED_SOURCES
    Source/Shared/CharacterHelper.cpp
    Source/Shared/CircleBuffer.cpp
    Source/Shared/GlobalFunctions.cpp
    Source/Shared/MACUtils.cpp
    Source/Shared/StdLibFileIO.cpp
    Source/Shared/WinFileIO.cpp)

set(LIBMAC_SOURCES
    Source/MACLib/APECompress.cpp
    Source/MACLib/APECompressCore.cpp
    Source/MACLib/APECompressCreate.cpp
    Source/MACLib/APEDecompress.cpp
    Source/MACLib/APEHeader.cpp
    Source/MACLib/APEInfo.cpp
    Source/MACLib/APELink.cpp
    Source/MACLib/APESimple.cpp
    Source/MACLib/APETag.cpp
    Source/MACLib/BitArray.cpp
    Source/MACLib/MACLib.cpp
    Source/MACLib/MACProgressHelper.cpp
    Source/MACLib/MD5.cpp
    Source/MACLib/NNFilter.cpp
    Source/MACLib/NewPredictor.cpp
    Source/MACLib/Prepare.cpp
    Source/MACLib/UnBitArray.cpp
    Source/MACLib/UnBitArrayBase.cpp
    Source/MACLib/WAVInputSource.cpp)

set(MAC_SOURCES
    Source/Console/Console.cpp)

set(SAMPLE1_SOURCES
    Analyze/Sample1/Sample1.cpp)

include_directories(
    ${PROJECT_BINARY_DIR}
    Source/Shared
    Source/MACLib)

if (APPLE)
    add_definitions(-DPLATFORM_APPLE)
elseif (WIN32)
    add_definitions(-DPLATFORM_WINDOWS)
else()
    add_definitions(-DPLATFORM_LINUX)
endif()

add_library(libmac
    ${SHARED_SOURCES}
    ${LIBMAC_SOURCES})

target_link_libraries(libmac ${NANOSLEEP_LIBRARY})

set_target_properties(libmac PROPERTIES
    OUTPUT_NAME mac)

add_executable(mac ${MAC_SOURCES})
target_link_libraries(mac libmac)

add_executable(Sample1 ${SAMPLE1_SOURCES})
target_link_libraries(Sample1 libmac)

install(TARGETS mac DESTINATION bin)
