cmake_minimum_required(VERSION 3.10)
project(zutty VERSION 0.12)

if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
  set(PLATFORM_DEFINES "-DBSD -DFREEBSD")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
  set(PLATFORM_DEFINES "-DBSD -DOPENBSD")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "NetBSD")
  set(PLATFORM_DEFINES "-DBSD")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(PLATFORM_DEFINES "-DLINUX")
endif()

add_executable(zutty
	src/charvdev.cc
	src/font.cc
	src/fontpack.cc
	src/frame.cc
	src/gl.cc
	src/log.cc
	src/main.cc
	src/options.cc
	src/pty.cc
	src/renderer.cc
	src/selmgr.cc
	src/vterm.cc
)

target_compile_options(zutty PUBLIC
	-std=c++20
	-Wall
	-Wextra
	-pedantic
	-Werror
	-Wno-variadic-macros
	-O3 -g
	-DZUTTY_VERSION=\"${PROJECT_VERSION}\"
	${PLATFORM_DEFINES}
	-I/usr/include/freetype2
	-I/usr/local/include/freetype2
	-I/usr/local/include
	-I/usr/include/libpng16)

target_link_options(zutty PUBLIC
	-pthread
	-L/usr/local/lib
	-lfreetype
	-lEGL
	-lGLESv2
	-lXmu
	-lXt
	-lX11)

