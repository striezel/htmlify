/*
 -------------------------------------------------------------------------------
    This file is part of htmlify.
    Copyright (C) 2012, 2013, 2016, 2024, 2025  Dirk Stolle

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
 -------------------------------------------------------------------------------
*/

#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <cstring>
#include "../pmdb/libstriezel/filesystem/file.hpp"
#ifndef NO_STRING_CONVERSION
#include "../pmdb/libstriezel/encoding/StringConversion.hpp"
#endif
#include "handleSpecialChars.hpp"
#include "../pmdb/code/MsgTemplate.hpp"
#include "../pmdb/code/bbcode/BBCode.hpp"
#include "../pmdb/code/bbcode/BBCodeParser.hpp"
#include "../pmdb/code/bbcode/DefaultCodes.hpp"
#include "../pmdb/code/bbcode/HorizontalRuleBBCode.hpp"
#include "../pmdb/code/bbcode/ListBBCode.hpp"
#include "../pmdb/code/bbcode/TableBBCode.hpp"
#include "../pmdb/code/bbcode/TableClasses.hpp"
#include "TrimmingBBCodes.hpp"
#include "htmlifyPostProcessors.hpp"

// return codes
const int rcInvalidParameter = 1;
const int rcFileError        = 2;
#ifndef NO_STRING_CONVERSION
const int rcConversionFail   = 3;
#endif

void showVersion()
{
  #ifndef NO_STRING_CONVERSION
  std::cout << "htmlify, version 0.08c, 2013-03-31\n";
  #else
  std::cout << "htmlify, version 0.08c~no-conv, 2013-03-31\n";
  #endif
  std::cout << '\n'
            << "Copyright (C) 2012, 2013  Dirk Stolle\n"
            << "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n"
            << "This is free software: you are free to change and redistribute it under the\n"
            << "terms of the GNU General Public License version 3 or any later version.\n"
            << "There is NO WARRANTY, to the extent permitted by law.\n";
}

void showHelp(const std::string& name)
{
  std::cout << "\n" << name << " [--html|--xhtml] FILENAME\n"
            << '\n'
            << "Converts BB code text input to proper HTML or XHTML snippets.\n"
            << '\n'
            << "options:\n"
            << "  --help           - Displays this help message and quits.\n"
            << "  -?               - same as --help\n"
            << "  --version        - Displays the version of the program and quits.\n"
            << "  -v               - same as --version\n"
            << "  FILENAME         - Adds file FILENAME to the list of text files that should\n"
            << "                     be processed. The file must exist, or the program will\n"
            << "                     abort. Can be repeated multiple times for different\n"
            << "                     files.\n"
            << "  --html           - Uses HTML (4.01) syntax for generated files. Enabled by\n"
            << "                     default.\n"
            << "  --xhtml          - Uses XHTML syntax for generated files. Mutually exclusive\n"
            << "                     with --html.\n"
            << "  --trim=PREFIX    - Removes PREFIX from link URLs, if they start with PREFIX.\n"
            #ifndef NO_STRING_CONVERSION
            << "  --utf8           - Content of input files is encoded in UTF-8 and will be\n"
            << "                     converted to ISO-8859-1 before processing.\n"
            #endif
            << "  --no-list        - Do not parse [LIST] codes when creating (X)HTML files.\n"
            << "  --br             - Convert new line characters to line breaks in (X)HTML\n"
            << "                     output. Disabled by default.\n"
            << "  --no-br          - Do not convert new line characters to line breaks in\n"
            << "                     (X)HTML output.\n"
            << "  --no-space-trim  - Do not reduce two or more consecutive spaces to one\n"
            << "                     single space character.\n"
            << "  --table=CLASS    - Sets the class for grids in <table> to CLASS.\n"
            << "  --row=CLASS      - Sets the class for grids in <tr> to CLASS.\n"
            << "  --cell=CLASS     - Sets the class for grids in <td> to CLASS.\n"
            << "  --std-classes    - Sets the 'standard' classes for the three class options.\n"
            << "                     This is equivalent to specifying all these parameters:\n"
            << "                         --table=" << TableClasses::DefaultTableClass << "\n"
            << "                         --row=" << TableClasses::DefaultRowClass << "\n"
            << "                         --cell=" << TableClasses::DefaultCellClass << "\n"
            << "  --max-table-width=WIDTH\n"
            << "                   - Sets the maximum width that is allowed for tables to\n"
            << "                     WIDTH pixels. Larger values will be discarded. Zero will\n"
            << "                     be interpreted as 'no limit'. The default value is 600.\n"
            << "  --no-table-limit - No default max. table width will be set by the program.\n"
            << "                     Mutually exclusive with --max-table-width=WIDTH.\n";
}

int main(int argc, char* argv[])
{
  std::set<std::string> pathTexts;
  HTMLStandard standard = HTMLStandard::HTML4_01;
  bool htmlModeSpecified = false;
  std::string trimmablePrefix = "";
  #ifndef NO_STRING_CONVERSION
  bool isUTF8 = false;
  #endif
  bool noList = false;
  bool hasSet_nl2br = false;
  bool nl2br = false;
  bool spaceTrim = true;
  TableClasses tableClasses(true);
  unsigned int tableLimit = 0;
  bool hasSetTableLimit = false;

  if ((argc > 1) && (argv != nullptr))
  {
    int i = 1;
    while (i < argc)
    {
      if (argv[i] != nullptr)
      {
        const std::string param = std::string(argv[i]);
        // help parameter
        if ((param == "--help") || (param == "-?") || (param == "/?"))
        {
          showHelp(argv[0]);
          return 0;
        }
        // version information requested?
        else if ((param == "--version") || (param == "-v"))
        {
          showVersion();
          return 0;
        }
        else if ((param == "--html") || (param == "--html4"))
        {
          if (htmlModeSpecified)
          {
            std::cerr << "Parameter " << param << " must not occur more than "
                      << "once and is mutually exclusive with --xhtml!\n";
            return rcInvalidParameter;
          }
          standard = HTMLStandard::HTML4_01;
          htmlModeSpecified = true;
        }
        else if ((param == "--xhtml") || (param == "--XHTML"))
        {
          if (htmlModeSpecified)
          {
            std::cerr << "Parameter " << param << " must not occur more than "
                      << "once and is mutually exclusive with --html!\n";
            return rcInvalidParameter;
          }
          standard = HTMLStandard::XHTML;
          htmlModeSpecified = true;
        }
        else if ((param == "-t") || (param == "--trim"))
        {
          if ((i+1 < argc) && (argv[i+1] != nullptr))
          {
            if (!trimmablePrefix.empty())
            {
              std::cerr << "Parameter " << param << " must not occur more than once!\n";
              return rcInvalidParameter;
            }
            trimmablePrefix = std::string(argv[i+1]);
            ++i; // skip next parameter, because it's used as prefix already
            std::cout << "Trimmable prefix was set to \"" << trimmablePrefix
                      << "\".\n";
          }
          else
          {
            std::cerr << "Error: You have to specify a string after \""
                      << param << "\".\n";
            return rcInvalidParameter;
          }
        }//param == trim
        else if ((param.substr(0,7) == "--trim=") && (param.length() > 7))
        {
          if (!trimmablePrefix.empty())
          {
            std::cerr << "Parameter --trim must not occur more than once!\n";
            return rcInvalidParameter;
          }
          trimmablePrefix = param.substr(7);
          std::cout << "Trimmable prefix was set to \"" << trimmablePrefix
                    << "\".\n";
        }//param == trim (single parameter version)
        else if ((param == "--utf8") || (param == "--UTF-8"))
        {
          #ifndef NO_STRING_CONVERSION
          if (isUTF8)
          {
            std::cerr << "Parameter " << param << " must not occur more than once!\n";
            return rcInvalidParameter;
          }
          isUTF8 = true;
          #else
          std::cerr << "Parameter " << param << " is not available in the no-conv build of htmlify!\n";
          return rcInvalidParameter;
          #endif
        }
        else if (param == "--no-list")
        {
          if (noList)
          {
            std::cerr << "Parameter --no-list must not occur more than once!\n";
            return rcInvalidParameter;
          }
          noList = true;
        }//param == no-list
        else if ((param == "--br") || (param == "--breaks"))
        {
          if (hasSet_nl2br)
          {
            std::cerr << "Parameter " << param << " must not occur more than once!\n";
            return rcInvalidParameter;
          }
          nl2br = true;
          hasSet_nl2br = true;
        }//param == br
        else if ((param == "--no-br") || (param == "--no-breaks"))
        {
          if (hasSet_nl2br)
          {
            std::cerr << "Parameter " << param << " must not occur more than once and is mutually exclusive with --br!\n";
            return rcInvalidParameter;
          }
          nl2br = false;
          hasSet_nl2br = true;
        }//param == br
        else if ((param == "--no-space-trim") || (param == "--leave-spaces-alone"))
        {
          if (!spaceTrim)
          {
            std::cerr << "Parameter " << param << " must not occur more than once!\n";
            return rcInvalidParameter;
          }
          spaceTrim = false;
        }//param == no-space-trim
        else if ((param.substr(0,8) == "--table=") && (param.length() > 8))
        {
          tableClasses.table = param.substr(8);
        }//param == 'table=...'
        else if ((param.substr(0,6) == "--row=") && (param.length() > 6))
        {
          tableClasses.row = param.substr(6);
        }//param == 'row=...'
        else if ((param.substr(0,7) == "--cell=") && (param.length() > 7))
        {
          tableClasses.cell = param.substr(7);
        }//param == 'cell=...'
        else if ((param == "--std-classes") || (param == "--classes") || (param == "--default-classes"))
        {
          tableClasses.table = TableClasses::DefaultTableClass;
          tableClasses.row   = TableClasses::DefaultRowClass;
          tableClasses.cell  = TableClasses::DefaultCellClass;
        }//param == std-classes
        else if ((param == "--width-limit") || (param == "--max-table-width"))
        {
          if ((i+1 < argc) && (argv[i+1] != nullptr))
          {
            if (hasSetTableLimit)
            {
              std::cerr << "Parameter " << param << " must not occur more than once!\n";
              return rcInvalidParameter;
            }
            unsigned int arg_val;
            if (!stringToUnsignedInt(std::string(argv[i+1]), arg_val))
            {
              std::cerr << "Error: \"" << std::string(argv[i+1]) << "\" is not a valid, positive integer!\n";
              return rcInvalidParameter;
            }
            tableLimit = arg_val;
            hasSetTableLimit = true;
            ++i; //skip next parameter, because it's used as limit already
            std::cout << "Table width limit was set to \"" << tableLimit << "\".\n";
          }
          else
          {
            std::cerr << "Error: You have to specify an integer value after \""
                      << param << "\".\n";
            return rcInvalidParameter;
          }
        }//param == max-table-width
        else if ((param.substr(0,18) == "--max-table-width=") && (param.length() > 18))
        {
          if (hasSetTableLimit)
          {
            std::cerr << "Parameter --max-table-width must not occur more than once!\n";
            return rcInvalidParameter;
          }
          unsigned int width_val;
          if (!stringToUnsignedInt(param.substr(18), width_val))
          {
            std::cerr << "Error: \"" << param.substr(18) << "\" is not a valid, positive integer!\n";
            return rcInvalidParameter;
          }
          tableLimit = width_val;
          hasSetTableLimit = true;
          std::cout << "Table width limit was set to \"" << tableLimit << "\".\n";
        }//param == max-table-width (single parameter version)
        else if (param == "--no-table-limit")
        {
          if (hasSetTableLimit)
          {
            std::cerr << "Parameter " << param << " must not occur more than once "
                      << "and is mutually exclusive with --max-table width!\n";
            return rcInvalidParameter;
          }
          tableLimit = 0;
          hasSetTableLimit = true;
          std::cout << "No table limit will be set.\n";
        }//param == no-table-limit
        else if (libstriezel::filesystem::file::exists(param))
        {
          if (pathTexts.find(param) != pathTexts.end())
          {
            std::cerr << "File \"" << param << "\" was specified more than once!\n";
            return rcInvalidParameter;
          }
          pathTexts.insert(param);
        }
        else
        {
          // unknown or wrong parameter
          std::cerr << "Invalid parameter given: \"" << param << "\".\n"
                    << "Use --help to get a list of valid parameters.\n";
          return rcInvalidParameter;
        }
      }//parameter exists
      else
      {
        std::cerr << "Parameter at index " << i << " is NULL.\n";
        return rcInvalidParameter;
      }
      ++i;//on to next parameter
    }//while
  }//if arguments present

  // no files to load?
  if (pathTexts.empty())
  {
    std::cerr << "You have to specify certain parameters for this programme to run properly.\n"
              << "Use --help to get a list of valid parameters.\n";
    return rcInvalidParameter;
  }

  if (tableClasses.table.empty()) tableClasses.table = TableClasses::DefaultTableClass;
  if (tableClasses.row.empty())   tableClasses.row   = TableClasses::DefaultRowClass;
  if (tableClasses.cell.empty())  tableClasses.cell  = TableClasses::DefaultCellClass;

  if (!hasSetTableLimit)
  {
    // set default value, if nothing has been set yet
    tableLimit = 600;
    std::cout << "Info: Table width limit was set to " << tableLimit << " by default.\n";
  }

  //prepare BB codes
  //image tags
  MsgTemplate tpl;
  tpl.loadFromString(standard == HTMLStandard::XHTML ? "<img src=\"{..inner..}\" alt=\"\" />"
                                                     : "<img src=\"{..inner..}\" alt=\"\">");
  SimpleTplAmpTransformBBCode img_simple("img", tpl, "inner");
  SimpleTrimBBCode img_simple_trim("img", tpl, "inner", trimmablePrefix);
  // advanced image tag
  tpl.loadFromString(standard == HTMLStandard::XHTML ? "<img src=\"{..inner..}\" align=\"{..attribute..}\" alt=\"\" />"
                                                     : "<img src=\"{..inner..}\" align=\"{..attribute..}\" alt=\"\">");
  AdvancedTplAmpTransformBBCode img_advanced("img", tpl, "inner", "attribute");
  AdvancedTrimBBCode img_advanced_trim("img", tpl, "inner", "attribute", trimmablePrefix);
  // simple url tag
  tpl.loadFromString("<a href=\"{..inner..}\" target=\"_blank\">{..inner..}</a>");
  SimpleTrimBBCode url_simple_trim("url", tpl, "inner", trimmablePrefix);
  // advanced url tag
  tpl.loadFromString("<a href=\"{..attribute..}\" target=\"_blank\">{..inner..}</a>");
  AdvancedTrimBBCode url_advanced_trim("url", tpl, "inner", "attribute", trimmablePrefix);
  // tag for unordered lists
  ListBBCode list_unordered("list", true);
  // tables
  TableBBCode table("table", tableClasses, tableLimit);
  // hr code
  HorizontalRuleBBCode hr("hr", standard);

  // add it to the parser
  BBCodeParser parser;
  parser.addCode(&bbcode_default::b);
  parser.addCode(&bbcode_default::u);
  parser.addCode(&bbcode_default::i);
  parser.addCode(&bbcode_default::s);
  parser.addCode(&bbcode_default::sup);
  parser.addCode(&bbcode_default::sub);
  parser.addCode(&bbcode_default::indent);
  parser.addCode(&bbcode_default::center);
  parser.addCode(&bbcode_default::left);
  parser.addCode(&bbcode_default::right);
  if (trimmablePrefix.empty())
  {
    parser.addCode(&img_simple);
    parser.addCode(&img_advanced);
    parser.addCode(&bbcode_default::url_simple);
    parser.addCode(&bbcode_default::url_advanced);
  }
  else
  {
    parser.addCode(&img_simple_trim);
    parser.addCode(&img_advanced_trim);
    parser.addCode(&url_simple_trim);
    parser.addCode(&url_advanced_trim);
  }
  parser.addCode(&bbcode_default::color);
  parser.addCode(&bbcode_default::size);
  parser.addCode(&bbcode_default::font);
  if (!noList) parser.addCode(&list_unordered);
  parser.addCode(&table);
  parser.addCode(&hr);

  NormalisingPreProcessor normaliser;
  KillSpacesBeforeNewline eatRedundantSpaces;
  ListNewlinePreProcessor preProc_List;
  ShortenDoubleSpaces preProc_Spaces;
  TablePreProcessor table_killLF("tr", "td");
  TablePostProcessor table_indent;
  TDR_PostProcessor tdr_post;

  if (nl2br) parser.addPreProcessor(&normaliser);
  parser.addPreProcessor(&eatRedundantSpaces);
  if (nl2br && !noList) parser.addPreProcessor(&preProc_List);
  if (spaceTrim) parser.addPreProcessor(&preProc_Spaces);
  if (nl2br) parser.addPreProcessor(&table_killLF);
  parser.addPostProcessor(&table_indent);
  parser.addPostProcessor(&tdr_post);

  for (const auto& path: pathTexts)
  {
    // read file contents
    std::ifstream input;
    input.open(path, std::ios_base::in | std::ios_base::binary);
    if (!input)
    {
      std::cerr << "Error: Could not open file \"" << path << "\"!\n";
      return rcFileError;
    }
    input.seekg(0, std::ios_base::end);
    const std::streamsize len = input.tellg();
    input.seekg(0, std::ios_base::beg);
    if (!input.good())
    {
      #ifdef DEBUG
      std::cerr << "Error while reading file content: seek operation failed!\n";
      #endif
      input.close();
      return rcFileError;
    }
    if (len > 1024*1024)
    {
      #ifdef DEBUG
      std::cerr << "Error while reading file content: unexpectedly large file size!\n";
      #endif
      input.close();
      return rcFileError;
    }
    // allocate buffer - all file content should fit into it
    char * buffer = new char[len + 1];
    memset(buffer, '\0', len + 1); //zerofill buffer
    input.read(buffer, len);
    if (!input.good())
    {
      delete[] buffer;
      buffer = nullptr;
      input.close();
      std::cerr << "Error while reading file content of \"" << path << "\"!\n";
      return rcFileError;
    }
    input.close();

    std::string content(buffer);
    delete[] buffer;
    buffer = nullptr;

    #ifndef NO_STRING_CONVERSION
    if (isUTF8)
    {
      // convert content to iso-8859-1
      std::string iso_content;
      if (!libstriezel::encoding::utf8_to_iso8859_1(content, iso_content))
      {
        std::cerr << "Error: Conversion from UTF-8 failed!\n";
        return rcConversionFail;
      }
      content = iso_content;
    } // if UTF-8
    #endif

    handleSpecialChars(content);
    content = parser.parse(content, "", standard, nl2br);

    // save content
    std::ofstream output;
    output.open(path + "_htmlified", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    if (!output)
    {
      std::cerr << "Could not open output file " << path << "_htmlified for writing!\n";
      return rcFileError;
    }
    output.write(content.c_str(), content.length());
    if (!output.good())
    {
      std::cerr << "Error while writing to file \"" << path << "_htmlified\"!\n";
      output.close();
      return rcFileError;
    }
    output.close();
    std::cout << "Processed file " << path << "\n";
  }
  return 0;
}
