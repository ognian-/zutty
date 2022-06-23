cmake_minimum_required(VERSION 3.10)
project(zutty VERSION 0.12)

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
	-fsanitize=thread
	-O3 -g
	-DZUTTY_VERSION=\"${PROJECT_VERSION}\"
	-I/usr/include/freetype2
	-I/usr/include/libpng16)

target_link_options(zutty PUBLIC
	-fsanitize=thread
	-pthread
	-lfreetype
	-lEGL
	-lGLESv2
	-lXmu
	-lXt
	-lX11)

