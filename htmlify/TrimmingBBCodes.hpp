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

#ifndef TRIMMINGBBCODES_HPP
#define TRIMMINGBBCODES_HPP

#include "../pmdb/code/bbcode/BBCode.hpp"
#include "../pmdb/code/bbcode/SimpleTplAmpTransformBBCode.hpp"
#include "../pmdb/code/bbcode/AdvancedTplAmpTransformBBCode.hpp"

std::string trimPrefix(std::string text, const std::string& prefix)
{
  const std::string::size_type prefix_len = prefix.length();
  if ((text.find(prefix) == 0) and (text.length() > prefix_len))
    text.erase(0, prefix_len);
  return text;
}


struct SimpleTrimBBCode: public SimpleTplAmpTransformBBCode
{
  public:
    /* constructor

       parameters:
           code   - "name" of the code, i.e. "b" for [B]bold text[/B]
           tpl    - the template that shall be used
           inner  - name of the template tag for the inner code
           prefix - prefix that should be trimmed, if present
    */
    SimpleTrimBBCode(const std::string& code, const MsgTemplate& tpl, const std::string& inner, const std::string& prefix)
    : SimpleTplAmpTransformBBCode(code, tpl, inner), m_Prefix(prefix)
    { }
  protected:
    /* applies a transformation (if any) to the inner content of the BB code
       during translation

       parameters:
           inner - the content
    */
    inline virtual std::string transformInner(const std::string& inner) const
    {
      return SimpleTplAmpTransformBBCode::transformInner(trimPrefix(inner, m_Prefix));
    }
  private:
    std::string m_Prefix;
};//struct


struct AdvancedTrimBBCode: public AdvancedTplAmpTransformBBCode
{
  public:
    /* constructor

       parameters:
           code   - "name" of the code, i.e. "b" for [B]bold text[/B]
           tpl    - the template that shall be used
           inner  - name of the template tag for the inner code
           attr   - name of the template tag for the attribute value
           prefix - prefix that should be trimmed, if present
    */
    AdvancedTrimBBCode(const std::string& code, const MsgTemplate& tpl, const std::string& inner, const std::string& attr, const std::string& prefix)
    : AdvancedTplAmpTransformBBCode(code, tpl, inner, attr), m_Prefix(prefix)
    { }
  protected:
    /* applies a transformation (if any) to the attribute value of the BB code
       during translation

       parameters:
           attr - the attribute value
    */
    inline virtual std::string transformAttribute(const std::string& attr) const
    {
      return AdvancedTplAmpTransformBBCode::transformInner(trimPrefix(attr, m_Prefix));
    }
  private:
    std::string m_Prefix;
};//struct

#endif // TRIMMINGBBCODES_HPP
