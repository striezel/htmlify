/*
 -------------------------------------------------------------------------------
    This file is part of htmlify.
    Copyright (C) 2012  Thoronador

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
#include "pmdb/libthoro/common/FileFunctions.h"
#ifndef NO_STRING_CONVERSION
#include "pmdb/libthoro/common/StringConversion.h"
#endif
#include "handleSpecialChars.h"
#include "pmdb/MsgTemplate.h"
#include "pmdb/bbcode/BBCode.h"
#include "pmdb/bbcode/BBCode_Table.h"
#include "pmdb/bbcode/BBCodeParser.h"
#include "pmdb/bbcode/DefaultCodes.h"
#include "TrimmingBBCodes.h"
#include "htmlifyPostProcessors.h"

//return codes
const int rcInvalidParameter = 1;
const int rcFileError        = 2;
#ifndef NO_STRING_CONVERSION
const int rcConversionFail   = 3;
#endif

void showGPLNotice()
{
  std::cout << "htmlify\n"
            << "  Copyright (C) 2012 Thoronador\n"
            << "\n"
            << "  This programme is free software: you can redistribute it and/or\n"
            << "  modify it under the terms of the GNU General Public License as published\n"
            << "  by the Free Software Foundation, either version 3 of the License, or\n"
            << "  (at your option) any later version.\n"
            << "\n"
            << "  This programme is distributed in the hope that they will be useful,\n"
            << "  but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
            << "  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
            << "  GNU General Public License for more details.\n"
            << "\n"
            << "  You should have received a copy of the GNU General Public License\n"
            << "  along with this programme.  If not, see <http://www.gnu.org/licenses/>.\n"
            << "\n";
}

void showVersion()
{
  showGPLNotice();
  #ifndef NO_STRING_CONVERSION
  std::cout << "htmlify, version 0.06b, 2012-10-31\n";
  #else
  std::cout << "htmlify, version 0.06b~no-conv, 2012-10-31\n";
  #endif
}

void showHelp(const std::string& name)
{
  std::cout << "\n"<<name<<" [--html|--xhtml] FILENAME\n"
            << "options:\n"
            << "  --help           - displays this help message and quits\n"
            << "  -?               - same as --help\n"
            << "  --version        - displays the version of the programme and quits\n"
            << "  -v               - same as --version\n"
            << "  FILENAME         - adds file FILENAME to the list of text files that should\n"
            << "                     be processed. The file must exist, or the programme will\n"
            << "                     abort. Can be repeated multiple times for different\n"
            << "                     files.\n"
            << "  --html           - uses HTML (4.01) syntax for generated files. Enabled by\n"
            << "                     default.\n"
            << "  --xhtml          - uses XHTML syntax for generated files. Mutually exclusive\n"
            << "                     with --html.\n"
            << "  --trim=PREFIX    - removes PREFIX from link URLs, if they start with PREFIX.\n"
            #ifndef NO_STRING_CONVERSION
            << "  --utf8           - content of input files is encoded in UTF-8 and will be\n"
            << "                     converted to ISO-8859-1 before processing.\n"
            #endif
            << "  --no-list        - do not parse [LIST] codes when creating (X)HTML files.\n"
            << "  --br             - convert new line characters to line breaks in (X)HTML\n"
            << "                     output. Disabled by default.\n"
            << "  --no-space-trim  - do not reduce two or more consecutive spaces to one\n"
            << "                     single space character.\n"
            << "  --table=CLASS    - sets the class for grids in <table> to CLASS.\n"
            << "  --row=CLASS      - sets the class for grids in <tr> to CLASS.\n"
            << "  --cell=CLASS     - sets the class for grids in <td> to CLASS.\n"
            << "  --std-classes    - sets the 'standard' classes for the three class options.\n"
            << "                     This is equivalent to specifying all these parameters:\n"
            << "                         --table="<<TableBBCode::DefaultTableClass<<"\n"
            << "                         --row="<<TableBBCode::DefaultRowClass<<"\n"
            << "                         --cell="<<TableBBCode::DefaultCellClass<<"\n";
}

int main(int argc, char **argv)
{
  std::set<std::string> pathTexts;
  bool doXHTML = false;
  bool htmlModeSpecified = false;
  std::string trimmablePrefix = "";
  #ifndef NO_STRING_CONVERSION
  bool isUTF8 = false;
  #endif
  bool noList = false;
  bool nl2br = false;
  bool spaceTrim = true;
  std::string classTable;
  std::string classRow;
  std::string classCell;

  if ((argc>1) and (argv!=NULL))
  {
    int i=1;
    while (i<argc)
    {
      if (argv[i]!=NULL)
      {
        const std::string param = std::string(argv[i]);
        //help parameter
        if ((param=="--help") or (param=="-?") or (param=="/?"))
        {
          showHelp(argv[0]);
          return 0;
        }//param == help
        //version information requested?
        else if ((param=="--version") or (param=="-v"))
        {
          showVersion();
          return 0;
        }//param == version
        else if ((param=="--html") or (param=="--html4"))
        {
          if (htmlModeSpecified)
          {
            std::cout << "Parameter "<<param<<" must not occur more than once "
                      << "and is mutually exclusive with --xhtml!\n";
            return rcInvalidParameter;
          }
          doXHTML = false;
          htmlModeSpecified = true;
        }//param == html
        else if ((param=="--xhtml") or (param=="--XHTML"))
        {
          if (htmlModeSpecified)
          {
            std::cout << "Parameter "<<param<<" must not occur more than once "
                      << "and is mutually exclusive with --html!\n";
            return rcInvalidParameter;
          }
          doXHTML = true;
          htmlModeSpecified = true;
        }//param == xhtml
        else if ((param=="-t") or (param=="--trim"))
        {
          if ((i+1<argc) and (argv[i+1]!=NULL))
          {
            if (!trimmablePrefix.empty())
            {
              std::cout << "Parameter "<<param<<" must not occur more than once!\n";
              return rcInvalidParameter;
            }
            trimmablePrefix = std::string(argv[i+1]);
            ++i; //skip next parameter, because it's used as prefix already
            std::cout << "Trimmable prefix was set to \""<<trimmablePrefix<<"\".\n";
          }
          else
          {
            std::cout << "Error: You have to specify a string after \""
                      << param <<"\".\n";
            return rcInvalidParameter;
          }
        }//param == trim
        else if ((param.substr(0,7)=="--trim=") and (param.length()>7))
        {
          if (!trimmablePrefix.empty())
          {
            std::cout << "Parameter --trim must not occur more than once!\n";
            return rcInvalidParameter;
          }
          trimmablePrefix = param.substr(7);
          std::cout << "Trimmable prefix was set to \""<<trimmablePrefix<<"\".\n";
        }//param == trim (single parameter version)
        else if ((param=="--utf8") or (param=="--UTF-8"))
        {
          #ifndef NO_STRING_CONVERSION
          if (isUTF8)
          {
            std::cout << "Parameter "<<param<<" must not occur more than once!\n";
            return rcInvalidParameter;
          }
          isUTF8 = true;
          #else
          std::cout << "Parameter "<<param<<" is not available in the no-conv build of htmlify!\n";
          return rcInvalidParameter;
          #endif
        }//param == utf8
        else if (param=="--no-list")
        {
          if (noList)
          {
            std::cout << "Parameter --no-list must not occur more than once!\n";
            return rcInvalidParameter;
          }
          noList = true;
        }//param == no-list
        else if ((param=="--br") or (param=="--breaks"))
        {
          if (nl2br)
          {
            std::cout << "Parameter "<<param<<" must not occur more than once!\n";
            return rcInvalidParameter;
          }
          nl2br = true;
        }//param == br
        else if ((param=="--no-space-trim") or (param=="--leave-spaces-alone"))
        {
          if (!spaceTrim)
          {
            std::cout << "Parameter "<<param<<" must not occur more than once!\n";
            return rcInvalidParameter;
          }
          spaceTrim = false;
        }//param == no-space-trim
        else if ((param.substr(0,8)=="--table=") and (param.length()>8))
        {
          classTable = param.substr(8);
        }//param == 'table=...'
        else if ((param.substr(0,6)=="--row=") and (param.length()>6))
        {
          classRow = param.substr(6);
        }//param == 'row=...'
        else if ((param.substr(0,7)=="--cell=") and (param.length()>7))
        {
          classCell = param.substr(7);
        }//param == 'cell=...'
        else if ((param=="--std-classes") or (param=="--classes") or (param=="--default-classes"))
        {
          classTable = TableBBCode::DefaultTableClass;
          classRow   = TableBBCode::DefaultRowClass;
          classCell  = TableBBCode::DefaultCellClass;
        }//param == std-classes
        else if (FileExists(param))
        {
          if (pathTexts.find(param)!=pathTexts.end())
          {
            std::cout << "File \""<<param<<"\" was specified more than once!\n";
            return rcInvalidParameter;
          }
          pathTexts.insert(param);
        }
        else
        {
          //unknown or wrong parameter
          std::cout << "Invalid parameter given: \""<<param<<"\".\n"
                    << "Use --help to get a list of valid parameters.\n";
          return rcInvalidParameter;
        }
      }//parameter exists
      else
      {
        std::cout << "Parameter at index "<<i<<" is NULL.\n";
        return rcInvalidParameter;
      }
      ++i;//on to next parameter
    }//while
  }//if arguments present

  //no files to load?
  if (pathTexts.empty())
  {
    std::cout << "You have to specify certain parameters for this programme to run properly.\n"
              << "Use --help to get a list of valid parameters.\n";
    return rcInvalidParameter;
  }

  if (classTable.empty()) classTable = TableBBCode::DefaultTableClass;
  if (classRow.empty())   classRow   = TableBBCode::DefaultRowClass;
  if (classCell.empty())  classCell  = TableBBCode::DefaultCellClass;

  //prepare BB codes
  //image tags
  CustomizedSimpleBBCode img_simple("img", "<img src=\"",
                                    doXHTML ? "\" alt=\"\" />" : "\" alt=\"\">");
  MsgTemplate tpl;
  tpl.loadFromString(doXHTML ? "<img src=\"{..inner..}\" alt=\"\" />"
                             : "<img src=\"{..inner..}\" alt=\"\">");
  SimpleTrimBBCode img_simple_trim("img", tpl, "inner", trimmablePrefix);
  //simple url tag
  tpl.loadFromString("<a href=\"{..inner..}\" target=\"_blank\">{..inner..}</a>");
  SimpleTrimBBCode url_simple_trim("url", tpl, "inner", trimmablePrefix);
  //advanced url tag
  tpl.loadFromString("<a href=\"{..attribute..}\" target=\"_blank\">{..inner..}</a>");
  AdvancedTrimBBCode url_advanced_trim("url", tpl, "inner", "attribute", trimmablePrefix);
  //tag for unordered lists
  ListBBCode list_unordered("list", true);
  //tables
  TableBBCode table("table", true, classTable, classRow, classCell);
  //hr code
  HorizontalRuleBBCode hr("hr", doXHTML);

  //add it to the parser
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
    parser.addCode(&bbcode_default::url_simple);
    parser.addCode(&bbcode_default::url_advanced);
  }
  else
  {
    parser.addCode(&img_simple_trim);
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
  TablePreprocessor table_killLF("tr", "td");
  TablePostProcessor table_indent;
  TDR_PostProcessor tdr_post;

  if (nl2br) parser.addPreProcessor(&normaliser);
  parser.addPreProcessor(&eatRedundantSpaces);
  if (nl2br and !noList) parser.addPreProcessor(&preProc_List);
  if (spaceTrim) parser.addPreProcessor(&preProc_Spaces);
  if (nl2br) parser.addPreProcessor(&table_killLF);
  parser.addPostProcessor(&table_indent);
  parser.addPostProcessor(&tdr_post);

  std::set<std::string>::const_iterator iter = pathTexts.begin();
  while (iter!=pathTexts.end())
  {
    //read file contents
    std::ifstream input;
    input.open(iter->c_str(), std::ios_base::in | std::ios_base::binary);
    if (!input)
    {
      std::cout << "Error: could not open file \""<<*iter<<"\"!\n";
      return rcFileError;
    }
    input.seekg(0, std::ios_base::end);
    const std::streamsize len = input.tellg();
    input.seekg(0, std::ios_base::beg);
    if (!input.good())
    {
      #ifdef DEBUG
      std::cout << "Error while reading file content: seek operation failed!\n";
      #endif
      input.close();
      return rcFileError;
    }
    if (len>1024*1024)
    {
      #ifdef DEBUG
      std::cout << "Error while reading file content: unexpectedly large file size!\n";
      #endif
      input.close();
      return rcFileError;
    }
    //allocate buffer - all file content should fit into it
    char * buffer = new char[len+1];
    memset(buffer, '\0', len+1); //zerofill buffer
    input.read(buffer, len);
    if (!input.good())
    {
      delete[] buffer;
      buffer = NULL;
      input.close();
      std::cout << "Error while reading file content of \""<<*iter<<"\"!\n";
      return rcFileError;
    }
    input.close();

    std::string content(buffer);
    delete[] buffer;
    buffer = NULL;

    #ifndef NO_STRING_CONVERSION
    if (isUTF8)
    {
      //convert content to iso-8859-1
      std::string iso_content;
      if (!Thoro::utf8_to_iso8859_1(content, iso_content))
      {
        std::cout << "Error: Conversion from UTF-8 failed!\n";
        return rcConversionFail;
      }
      content = iso_content;
    }//if UTF-8
    #endif

    handleSpecialChars(content);
    content = parser.parse(content, "", doXHTML, nl2br);

    //save content
    std::ofstream output;
    output.open((*iter + "_htmlified").c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    if (!output)
    {
      std::cout << "Could not open output file "<<*iter<<"_htmlified for writing!\n";
      return rcFileError;
    }
    output.write(content.c_str(), content.length());
    if (!output.good())
    {
      std::cout << "Error while writing to file \""<<*iter<<"_htmlified\"!\n";
      output.close();
      return rcFileError;
    }
    output.close();
    std::cout << "Processed file "<<*iter<<"\n";
    ++iter;
  }//while
  return 0;
}
