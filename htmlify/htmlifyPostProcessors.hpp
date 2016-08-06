/*
 -------------------------------------------------------------------------------
    This file is part of htmlify.
    Copyright (C) 2012  Dirk Stolle

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

#ifndef HTMLIFYPOSTPROCESSORS_HPP
#define HTMLIFYPOSTPROCESSORS_HPP

#include "../pmdb/code/bbcode/TextProcessor.hpp"

/* struct NormalisingPreProcessor:
      "normalises" line breaks by replacing \r\n (Windows style) by \n (Unix style)
*/
struct NormalisingPreProcessor: public TextProcessor
{
  public:
    /* processes the given text, i.e. performs transformations

       parameters:
           text - the message text that should be processed
    */
    virtual void applyToText(std::string& text) const
    {
      std::string needle = "\r\n";
      std::string::size_type pos = text.find(needle);
      while (pos!=std::string::npos)
      {
        text.replace(pos, needle.length(), "\n");
        pos = text.find(needle, pos);
      }//while
    }
};//struct

/* struct TablePostProcessor:
      adds my usual indentation to table codes
*/
struct TablePostProcessor: public TextProcessor
{
  public:
    /* processes the given text, i.e. performs transformations

       parameters:
           text - the message text that should be processed
    */
    virtual void applyToText(std::string& text) const
    {
      std::string needle = "\n<tr class=\"grid_tr\">\n";
      std::string::size_type pos = text.find(needle);
      while (pos!=std::string::npos)
      {
        text.replace(pos, needle.length(), "\n  <tr class=\"grid_tr\">\n");
        pos = text.find(needle, pos+10);
      }//while

      needle = "\n</tr>\n";
      pos = text.find(needle);
      while (pos!=std::string::npos)
      {
        text.replace(pos, needle.length(), "\n  </tr>\n");
        pos = text.find(needle, pos+12);
      }//while

      needle = "\n<td class=\"grid_td\">";
      pos = text.find(needle);
      while (pos!=std::string::npos)
      {
        text.replace(pos, needle.length(), "\n    <td class=\"grid_td\">");
        pos = text.find(needle, pos+12);
      }//while

      needle = "><tr";
      pos = text.find(needle);
      while (pos!=std::string::npos)
      {
        text.replace(pos, needle.length(), ">\n  <tr");
        pos = text.find(needle, pos+6);
      }//while

      needle = "><td";
      pos = text.find(needle);
      while (pos!=std::string::npos)
      {
        text.replace(pos, needle.length(), ">\n    <td");
        pos = text.find(needle, pos+9);
      }//while

      needle = "></tr>";
      pos = text.find(needle);
      while (pos!=std::string::npos)
      {
        text.replace(pos, needle.length(), ">\n  </tr>");
        pos = text.find(needle, pos+9);
      }//while

      needle = "></table>";
      pos = text.find(needle);
      while (pos!=std::string::npos)
      {
        text.replace(pos, needle.length(), ">\n</table>");
        pos = text.find(needle, pos+10);
      }//while
    }
};//struct

/* struct TDR_PostProcessor:
      replaces stuff like
        "<center>\n
         some stuff\n
         </center>"
      with
        "\n
         <center>some stuff</center>\n
         "
*/
struct TDR_PostProcessor: public TextProcessor
{
  public:
    /* processes the given text, i.e. performs transformations

       parameters:
           text - the message text that should be processed
    */
    virtual void applyToText(std::string& text) const
    {
      std::string needle = "<center>\n";
      std::string::size_type pos = text.find(needle);
      while (pos!=std::string::npos)
      {
        text.replace(pos, needle.length(), "\n<center>");
        pos = text.find(needle, pos+8);
      }//while

      needle = "\n</center>";
      pos = text.find(needle);
      while (pos!=std::string::npos)
      {
        text.replace(pos, needle.length(), "</center>\n");
        pos = text.find(needle, pos+8);
      }//while
    }
};//struct

#endif // HTMLIFYPOSTPROCESSORS_HPP
