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

#ifndef HANDLESPECIALCHARS_HPP
#define HANDLESPECIALCHARS_HPP

#include <string>
#include <map>

void handleSpecialChars(std::string& text)
{
  std::map<std::string, std::string> specChars;
  specChars["Ä"] = "&Auml;"; specChars["ä"] = "&auml;";
  specChars["Ö"] = "&Ouml;"; specChars["ö"] = "&ouml;";
  specChars["Ü"] = "&Uuml;"; specChars["ü"] = "&uuml;";
  specChars["ß"] = "&szlig;";

  std::map<std::string, std::string>::const_iterator map_iter = specChars.begin();
  while (map_iter!=specChars.end())
  {
    std::string::size_type pos = text.find(map_iter->first);
    const std::string::size_type len = map_iter->first.length();
    while (pos!=std::string::npos)
    {
      text.replace(pos, len, map_iter->second);
      pos = text.find(map_iter->first, pos);
    }//while (inner)
    ++map_iter;
  }//while specChars
}

#endif // HANDLESPECIALCHARS_HPP
