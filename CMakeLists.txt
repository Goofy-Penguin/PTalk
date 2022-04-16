cmake_minimum_required (VERSION 3.8)

include(FetchContent)

# version
set(PTALK_VERSION_MAJOR 0 CACHE STRING "major version" FORCE)
set(PTALK_VERSION_MINOR 1 CACHE STRING "minor version" FORCE)
set(PTALK_VERSION ${PTALK_VERSION_MAJOR}.${PTALK_VERSION_MINOR} CACHE STRING "version" FORCE)

# flags
set(BUILD_TESTING OFF)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSPDLOG_FMT_EXTERNAL")

IF(MSVC)
	SET(CMAKE_CXX_FLAGS "/EHsc")
ENDIF(MSVC)

# msvsc needs this as cmake doesn't provice for it yet
# TODO: once cmake supports msvsc std latest remove this
if (MSVC_VERSION GREATER_EQUAL "1900")
	include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
	if (_cpp_latest_flag_supported)
		add_compile_options("/std:c++latest")
	endif()
endif()

# libraries
FetchContent_GetProperties(mainframe)
if(NOT mainframe_POPULATED)
	FetchContent_Declare(mainframe GIT_REPOSITORY https://github.com/Goofy-Penguin/Mainframe.git)
	FetchContent_MakeAvailable(mainframe)
endif()

project ("ptalk" VERSION ${PTALK_VERSION})

# conan for libaries
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                  "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_CURRENT_BINARY_DIR}/conan.cmake)

if(CMAKE_COMPILER_IS_GNUXX)
   set(CONAN_SETTINGS compiler.libcxx=libstdc++11)
endif()

conan_cmake_run(CONANFILE conanfile.txt
                SETTINGS ${CONAN_SETTINGS}
                BUILD missing)

include(${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake)

find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)

if (WIN32)
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")
endif()

add_subdirectory("code")
