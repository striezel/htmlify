# We might support earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

project(htmlify)

add_definitions(-DNO_SMILIES_IN_PARSER)

set(htmlify_sources
    main.cpp
    pmdb/MsgTemplate.cpp
    pmdb/bbcode/BBCode.cpp
    pmdb/bbcode/BBCodeParser.cpp
    pmdb/bbcode/BBCode_Table.cpp
    pmdb/bbcode/quotes.cpp
    pmdb/libthoro/common/FileFunctions.cpp
    pmdb/libthoro/common/StringUtils.cpp
    main.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions (-Wall -O3 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(htmlify ${htmlify_sources})
