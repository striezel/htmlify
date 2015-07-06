# htmlify

htmlify is a small command-line tool that can be used to transform BB code
text input (as seen in some bulletin board softwares) to proper HTML or XHTML
snippets.
The text is read from one or more input file(s), which have to be specified as
command-line arguments to the program.

# Documentation

There is no documentation yet - apart from this readme file.

# Building htmlify from source

## Prerequisites

To build htmlify from source you need a C++ compiler and CMake 2.8 or later.
It also helps to have Git, a distributed version control system, on your build
system to get the latest source code directly from the Git repository.

All three can usually be installed be typing

  apt-get install cmake g++ git

or

  yum install cmake gcc-c++ git

into a root terminal.

## Getting the source code

Get the source directly from Git by cloning the Git repository and change to
the directory after the repository is completely cloned:

  git clone https://github.com/Thoronador/htmlify.git ./htmlify
  cd htmlify

## Build process

The build process is relatively easy, because CMake does all the preparations.
Starting in the root directory of the source, you can do the following steps:

  mkdir build
  cd build
  cmake ../
  make -j2

Now the htmlify binary is built and ready for use.

# Installing htmlify via package manager

There are no .deb or .rpm packages yet, so the build from source is currently
the only way to get htmlify.

# Copyright and Licensing

Copyright 2012-2015 Thoronador

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.