cmake_minimum_required (VERSION 3.8)

project(htmlify)

add_definitions(-DNO_SMILIES_IN_PARSER)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions (-Wall -O3 -fexceptions)
endif ()

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(htmlify ${htmlify_sources})
