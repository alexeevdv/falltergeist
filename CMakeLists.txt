#
# Copyright 2012-2018 Falltergeist Developers.
#
# This file is part of Falltergeist.
#
# Falltergeist is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Falltergeist is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Falltergeist.  If not, see <http://www.gnu.org/licenses/>.
#

# It must be set before project
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build (by default Debug)")

cmake_minimum_required(VERSION 3.1)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)

project(falltergeist VERSION 0.4.0)

include_directories(src)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	set(USE_CONAN TRUE)
endif()

if (USE_CONAN)
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup()
endif()

find_package(ZLIB REQUIRED)
if(NOT ZLIB_FOUND)
	message(FATAL_ERROR "zlib library not found")
endif(NOT ZLIB_FOUND)
include_directories(${ZLIB_INCLUDE_DIRS})

find_package(SDL2 REQUIRED)
if(NOT SDL2_FOUND)
	message(FATAL_ERROR "SDL2 library not found")
endif(NOT SDL2_FOUND)
include_directories(${SDL2_INCLUDE_DIR})

find_package(SDL2_mixer REQUIRED)
if(NOT SDLMIXER_FOUND)
	message(FATAL_ERROR "SDL2_mixer library not found")
endif(NOT SDLMIXER_FOUND)
include_directories(${SDL_MIXER_INCLUDE_DIR})

find_package(SDL2_image REQUIRED)
if(NOT SDLIMAGE_FOUND)
	message(FATAL_ERROR "SDL2_image library not found")
endif(NOT SDLIMAGE_FOUND)
include_directories(${SDL_IMAGE_INCLUDE_DIR})

find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
	message(FATAL_ERROR "GL library not found")
endif(NOT OPENGL_FOUND)
include_directories(${OPENGL_INCLUDE_DIR})

if (NOT USE_CONAN)
	find_package(GLEW REQUIRED)
	if(NOT GLEW_FOUND)
		message(FATAL_ERROR "GLEW library not found")
	endif(NOT GLEW_FOUND)
	include_directories(${GLEW_INCLUDE_DIR})
endif()

find_package(GLM REQUIRED)
if(NOT GLM_FOUND)
	message(FATAL_ERROR "GLM library not found")
endif(NOT GLM_FOUND)
include_directories(${GLM_INCLUDE_DIR})

file(GLOB_RECURSE SOURCES  src/*.cpp)

if(MSVC)
	set(_additional_includes "package/windows.rc" pchheader.cpp)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
configure_file (
	"${PROJECT_SOURCE_DIR}/src/Version.h.in"
	"${PROJECT_BINARY_DIR}/src/Version.h"
)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)
add_executable(falltergeist main.cpp ${SOURCES} ${_additional_includes})

set_target_properties(falltergeist PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
)

if(MSVC)
	set_property(TARGET falltergeist
		APPEND PROPERTY LINK_FLAGS /NODEFAULTLIB:MSVCRT
	)
	include(cmake/modules/PrecompiledHeader.cmake)
	add_precompiled_header(falltergeist pchheader.h FORCEINCLUDE SOURCE_CXX pchheader.cpp)
else()
	add_definitions(-Wall)
endif()

if (CONAN_LIBS)
	target_link_libraries(falltergeist ${CONAN_LIBS})
else()
	target_link_libraries(falltergeist ${ZLIB_LIBRARIES} ${SDL2_LIBRARY} ${SDL_MIXER_LIBRARY} ${SDL_IMAGE_LIBRARY} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY})
endif()

include(cmake/install/windows.cmake)
include(cmake/install/linux.cmake)
include(cmake/install/apple.cmake)
