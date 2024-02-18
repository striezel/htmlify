/*
 -------------------------------------------------------------------------------
    This file is part of htmlify.
    Copyright (C) 2012, 2016, 2024  Dirk Stolle

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
  // some "special" characters used in German
  const std::map<char, std::string> specChars = {
    {'\xC4', "&Auml;"},
    {'\xE4', "&auml;"},
    {'\xD6', "&Ouml;"},
    {'\xF6', "&ouml;"},
    {'\xDC', "&Uuml;"},
    {'\xFC', "&uuml;"},
    {'\xDF', "&szlig;"}
  };

  for(const auto & [special_char, replacement] : specChars)
  {
    std::string::size_type pos = text.find(special_char);
    while (pos != std::string::npos)
    {
      text.replace(pos, 1, replacement);
      pos = text.find(special_char, pos);
    }
  }
}

#endif // HTMLIFY_HANDLESPECIALCHARS_HPP
