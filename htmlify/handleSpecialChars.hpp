/*
 -------------------------------------------------------------------------------
    This file is part of htmlify.
    Copyright (C) 2012, 2016  Dirk Stolle

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

#ifndef HTMLIFY_HANDLESPECIALCHARS_HPP
#define HTMLIFY_HANDLESPECIALCHARS_HPP

#include <string>
#include <map>

void handleSpecialChars(std::string& text)
{
  std::map<std::string, std::string> specChars;
  //some "special" characters used in German
  specChars["Ä"] = "&Auml;";
  specChars["ä"] = "&auml;";
  specChars["Ö"] = "&Ouml;";
  specChars["ö"] = "&ouml;";
  specChars["Ü"] = "&Uuml;";
  specChars["ü"] = "&uuml;";
  specChars["ß"] = "&szlig;";

  for(auto & item : specChars)
  {
    std::string::size_type pos = text.find(item.first);
    const std::string::size_type len = item.first.length();
    while (pos != std::string::npos)
    {
      text.replace(pos, len, item.second);
      pos = text.find(item.first, pos);
    }//while
  }//for (range-based)
}

#endif // HTMLIFY_HANDLESPECIALCHARS_HPP
