/***********************************************************************
* fvtermbuffer.cpp - Buffer for virtual terminal strings               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2021 Markus Gans                                      *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <algorithm>
#include <string>
#include <vector>

#include "final/fc.h"
#include "final/ftypes.h"
#include "final/output/foutput.h"
#include "final/output/tty/ftermdata.h"
#include "final/util/fstring.h"
#include "final/vterm/fcolorpair.h"
#include "final/vterm/fstyle.h"
#include "final/vterm/fvtermbuffer.h"
#include "final/vterm/fvterm.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FVTermBuffer
//----------------------------------------------------------------------

// public methods of FVTermBuffer
//----------------------------------------------------------------------
FString FVTermBuffer::toString() const
{
  std::wstring wide_string{};
  wide_string.reserve(data.size());
  std::for_each ( data.cbegin()
                , data.cend()
                , [&wide_string] (const FChar& fchar)
                  {
                    for (auto&& ch : fchar.ch)
                    {
                      if ( ch == L'\0' )
                        return;

                      wide_string.push_back(ch);
                    }
                  }
                );
  return wide_string;
}

//----------------------------------------------------------------------
int FVTermBuffer::print (const FString& string)
{
  data.reserve(data.size() + string.getLength());
  const auto last = string.cend();
  auto cbegin = string.cbegin();
  auto iter = cbegin;
  int char_width{0};

  for (auto&& ch : string)
  {
    auto width = getColumnWidth(ch);
    auto wspace = std::iswspace(wint_t(ch));

    if ( width == 0 && ! wspace )  // zero-width character
    {
      if ( iter == cbegin)
        ++cbegin;

      ++iter;
    }
    else if ( iter != cbegin )
      add(cbegin, iter, char_width);

    if ( iter == cbegin && (width > 0 || is7bit(ch)) )  // 1st char
      ++iter;

    if ( width > 0 )
      char_width += width;

    if ( wspace )
      add(cbegin, iter, char_width);
  }

  if ( iter == last )
    add(cbegin, iter, char_width);

  return int(string.getLength());
}

//----------------------------------------------------------------------
int FVTermBuffer::print (wchar_t ch)
{
  FChar nc{FVTermAttribute::getAttribute()};  // next character
  nc.ch[0] = ch;
  addColumnWidth(nc);  // add column width
  nc.attr.bit.no_changes = false;
  nc.attr.bit.printed = false;
  data.emplace_back(nc);
  return 1;
}

//----------------------------------------------------------------------
void FVTermBuffer::print (const FStyle& style) const
{
  FVTermAttribute::print(style);
}

//----------------------------------------------------------------------
void FVTermBuffer::print (const FColorPair& pair) const
{
  FVTermAttribute::setColor(pair.getForegroundColor(), pair.getBackgroundColor());
}


// private methods of FVTermBuffer
//----------------------------------------------------------------------
void FVTermBuffer::add ( FString::const_iterator& cbegin
                       , const FString::const_iterator& cend
                       , int& char_width )
{
  static const auto& fterm_data = FTermData::getInstance();

  if ( cbegin == cend )
    return;

  FChar nc{FVTermAttribute::getAttribute()};  // next character
  nc.attr.byte[2] = 0;
  nc.attr.byte[3] = 0;


  if ( char_width == 2
    && fterm_data.getTerminalEncoding() != Encoding::UTF8 )
  {
    nc.ch[0] = '.';
    nc.attr.bit.char_width = 1;
  }
  else
    nc.attr.bit.char_width = char_width & 0x03;

  std::copy(cbegin, std::min(cend, cbegin + UNICODE_MAX), nc.ch.begin());
  data.emplace_back(nc);
  cbegin = cend;
  char_width = 0;
}


// FVTermBuffer non-member operators
//----------------------------------------------------------------------
FVTermBuffer::FCharVector& operator << ( FVTermBuffer::FCharVector& term_string
                                       , const FVTermBuffer& buf )
{
  if ( ! buf.data.empty() )
    term_string.assign(buf.data.cbegin(), buf.data.cend());

  return term_string;
}

}  // namespace finalcut
