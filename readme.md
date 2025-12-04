# htmlify

[![GitLab pipeline status](https://gitlab.com/striezel/htmlify/badges/master/pipeline.svg)](https://gitlab.com/striezel/htmlify/-/pipelines)
[![GitHub CI Clang status](https://github.com/striezel/htmlify/workflows/Clang/badge.svg)](https://github.com/striezel/htmlify/actions)
[![GitHub CI GCC status](https://github.com/striezel/htmlify/workflows/GCC/badge.svg)](https://github.com/striezel/htmlify/actions)
[![GitHub CI MSYS2 status](https://github.com/striezel/htmlify/workflows/MSYS2/badge.svg)](https://github.com/striezel/htmlify/actions)

htmlify is a small command-line tool that can be used to transform BB code
text input (as seen in some bulletin board software) to proper HTML or XHTML
snippets.
The text is read from one or more input file(s), which have to be specified as
command-line arguments to the program.

## Usage

```
htmlify [--html|--xhtml] FILENAME

Converts BB code text input to proper HTML or XHTML snippets.

options:
  --help           - Displays this help message and quits.
  -?               - same as --help
  --version        - Displays the version of the program and quits.
  -v               - same as --version
  FILENAME         - Adds file FILENAME to the list of text files that should
                     be processed. The file must exist, or the program will
                     abort. Can be repeated multiple times for different
                     files.
  --html           - Uses HTML (4.01) syntax for generated files. Enabled by
                     default.
  --xhtml          - Uses XHTML syntax for generated files. Mutually exclusive
                     with --html.
  --trim=PREFIX    - Removes PREFIX from link URLs, if they start with PREFIX.
  --utf8           - Content of input files is encoded in UTF-8 and will be
                     converted to ISO-8859-1 before processing.
  --no-list        - Do not parse [LIST] codes when creating (X)HTML files.
  --br             - Convert new line characters to line breaks in (X)HTML
                     output. Disabled by default.
  --no-br          - Do not convert new line characters to line breaks in
                     (X)HTML output.
  --no-space-trim  - Do not reduce two or more consecutive spaces to one
                     single space character.
  --table=CLASS    - Sets the class for grids in <table> to CLASS.
  --row=CLASS      - Sets the class for grids in <tr> to CLASS.
  --cell=CLASS     - Sets the class for grids in <td> to CLASS.
  --std-classes    - Sets the 'standard' classes for the three class options.
                     This is equivalent to specifying all these parameters:
                         --table=grid_table
                         --row=grid_tr
                         --cell=grid_td
  --max-table-width=WIDTH
                   - Sets the maximum width that is allowed for tables to
                     WIDTH pixels. Larger values will be discarded. Zero will
                     be interpreted as 'no limit'. The default value is 600.
  --no-table-limit - No default max. table width will be set by the program.
                     Mutually exclusive with --max-table-width=WIDTH.
```

## Building htmlify from source

### Prerequisites

To build htmlify from source you need a C++ compiler with support for C++17 and
CMake 3.8 or later.

It also helps to have Git, a distributed version control system, on your build
system to get the latest source code directly from the Git repository.

All three can usually be installed by typing

    apt-get install cmake g++ git

or

    yum install cmake gcc-c++ git

into a root terminal.

### Getting the source code

Get the source directly from Git by cloning the Git repository and change to
the directory after the repository is completely cloned:

    git clone https://gitlab.com/striezel/htmlify.git ./htmlify
    cd htmlify
    git submodule update --init --recursive

The last of the lines above initializes and updates the submodules that the
htmlify source code needs, too, to be build from source.

### Build process

The build process is relatively easy, because CMake does all the preparations.
Starting in the root directory of the source, you can do the following steps:

    mkdir build
    cd build
    cmake ../
    cmake --build . -j2

Now the htmlify binary is built and ready for use.

## Copyright and Licensing

Copyright 2012-2015 Dirk Stolle

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
