# We might support earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

project(htmlify)

add_definitions(-DNO_SMILIES_IN_PARSER -std=c++0x)

set(htmlify_sources
    ../pmdb/code/MsgTemplate.cpp
    ../pmdb/code/bbcode/AdvancedTemplateBBCode.cpp
    ../pmdb/code/bbcode/BBCodeParser.cpp
    ../pmdb/code/bbcode/CustomizedSimpleBBCode.cpp
    ../pmdb/code/bbcode/HorizontalRuleBBCode.cpp
    ../pmdb/code/bbcode/ListBBCode.cpp
    ../pmdb/code/bbcode/SimpleBBCode.cpp
    ../pmdb/code/bbcode/SimpleTemplateBBCode.cpp
    ../pmdb/code/bbcode/TableBBCode.cpp
    ../pmdb/code/bbcode/quotes.cpp
    ../pmdb/libstriezel/common/StringUtils.cpp
    ../pmdb/libstriezel/encoding/StringConversion.cpp
    ../pmdb/libstriezel/filesystem/file.cpp
    main.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions (-Wall -O3 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(htmlify ${htmlify_sources})
