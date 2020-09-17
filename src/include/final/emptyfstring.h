/***********************************************************************
* emptyfstring.h - Creates an empty FString object                     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2020 Markus Gans                                      *
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

/*  Standalone class
 *  ════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ emptyFString ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef EMPTYFSTRING_H
#define EMPTYFSTRING_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fapplication.h"
#include "final/flog.h"
#include "final/fstring.h"

namespace finalcut
{

namespace fc
{

//----------------------------------------------------------------------
// class emptyFString
//----------------------------------------------------------------------

class emptyFString final
{
public:
  // Constructors
  emptyFString() = delete;

  // Disable copy constructor
  emptyFString (const emptyFString&) = delete;

  // Disable copy assignment operator (=)
  emptyFString& operator = (const emptyFString&) = delete;

  static const FString getClassName();
  static bool isNull();
  static const FString& get();
  static void clear();

private:
  // Data member
  static const FString* empty_string;
};

// emptyFString inline functions
//----------------------------------------------------------------------
inline const FString emptyFString::getClassName()
{ return "emptyFString"; }

//----------------------------------------------------------------------
inline bool emptyFString::isNull()
{
  return ( empty_string ) ? false : true;
}

//----------------------------------------------------------------------
inline const FString& emptyFString::get()
{
  if ( ! empty_string )
  {
    try
    {
      empty_string = new FString("");
    }
    catch (const std::bad_alloc&)
    {
      badAllocOutput ("FString");
    }
  }

  return *empty_string;
}

//----------------------------------------------------------------------
inline void emptyFString::clear()
{
  delete empty_string;
  empty_string = nullptr;
}

}  // namespace fc

}  // namespace finalcut

#endif  // EMPTYFSTRING_H
