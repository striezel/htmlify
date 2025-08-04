cmake_minimum_required (VERSION 3.8...3.31)

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
    ../pmdb/code/bbcode/TableClasses.cpp
    ../pmdb/code/bbcode/quotes.cpp
    ../pmdb/libstriezel/common/StringUtils.cpp
    ../pmdb/libstriezel/encoding/StringConversion.cpp
    ../pmdb/libstriezel/filesystem/file.cpp
    main.cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions (-Wall -Wextra -Wshadow -Wpedantic -pedantic-errors -O3 -fexceptions)

    set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )
endif ()

add_executable(htmlify ${htmlify_sources})

if (MINGW)
  # find iconv library
  find_package(PkgConfig)
  pkg_search_module(ICONV REQUIRED iconv)
  if (ICONV_FOUND)
    include_directories(${ICONV_INCLUDE_DIRS})
    target_link_libraries (htmlify ${ICONV_LIBRARIES})
  else ()
    message ( FATAL_ERROR "libiconv was not found!")
  endif ()
endif ()


# #################### #
# tests for executable #
# #################### #

# add tests for --version and --help parameters
# default help parameter "--help"
add_test(NAME htmlify_help
         COMMAND $<TARGET_FILE:htmlify> --help)

# short help parameter with question mark "-?"
add_test(NAME htmlify_help_question_mark
         COMMAND $<TARGET_FILE:htmlify> -?)

# Windows-style help parameter "/?"
if (NOT DEFINED ENV{GITHUB_ACTIONS} OR NOT MINGW)
    add_test(NAME htmlify_help_question_mark_windows
             COMMAND $<TARGET_FILE:htmlify> /?)
endif ()

# parameter to show version number
add_test(NAME htmlify_version
         COMMAND $<TARGET_FILE:htmlify> --version)
