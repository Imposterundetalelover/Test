/***********************************************************************
* ftermfreebsd.cpp - Contains the FreeBSD terminal functions           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2020 Markus Gans                                      *
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

#include "final/fapplication.h"
#include "final/fcharmap.h"
#include "final/flog.h"
#include "final/fsystem.h"
#include "final/fterm.h"
#include "final/ftermdata.h"
#include "final/ftermfreebsd.h"
#include "final/ftypes.h"

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
#define initCheck(ret_value)           \
    if ( ! isInitialized() )           \
    {                                  \
      if ( ! FApplication::isQuit() )  \
        warnNotInitialized();          \
                                       \
      return ret_value;                \
    }
#endif

namespace finalcut
{

// static class attributes
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  uInt                      FTermFreeBSD::bsd_alt_keymap{0};
  FTermFreeBSD::CursorStyle FTermFreeBSD::cursor_style{fc::normal_cursor};
  bool                      FTermFreeBSD::change_cursorstyle{true};
  bool                      FTermFreeBSD::meta_sends_escape{true};
  FSystem*                  FTermFreeBSD::fsystem{nullptr};
  FTermData*                FTermFreeBSD::fterm_data{nullptr};
#endif


//----------------------------------------------------------------------
// class FTermFreeBSD
//----------------------------------------------------------------------

// public methods of FTermFreeBSD
//----------------------------------------------------------------------
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
FTermFreeBSD::CursorStyle FTermFreeBSD::getCursorStyle()
{
  return cursor_style;
}

//----------------------------------------------------------------------
bool FTermFreeBSD::setCursorStyle (CursorStyle style)
{
  // Set cursor style in a BSD console

  if ( ! fterm_data )
    fterm_data = FTerm::getFTermData();

  initCheck(false);

  if ( ! fsystem || ! isFreeBSDConsole() || ! change_cursorstyle )
    return false;

  cursor_style = style;

  if ( fterm_data->isCursorHidden() )
    return false;

  return setFreeBSDCursorStyle(style);
}

//----------------------------------------------------------------------
bool FTermFreeBSD::isFreeBSDConsole()
{
  // Check if it's a FreeBSD console

  keymap_t keymap{};

  if ( ! fsystem )
    fsystem = FTerm::getFSystem();

  if ( fsystem && fsystem->ioctl(0, GIO_KEYMAP, &keymap) == 0 )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
void FTermFreeBSD::setBeep (int Hz, int ms)
{
  if ( ! FTerm::isFreeBSDTerm() )
    return;

  // Range for frequency: 21-32766
  if ( Hz < 21 || Hz > 32766 )
    return;

  // Range for duration:  0-1999
  if ( ms < 0 || ms > 1999 )
    return;

  constexpr int timer_frequency = 1193182;
  int period = timer_frequency / Hz;
  ms /= 10;
  FTerm::putstringf (CSI "=%d;%dB", period, ms);
  std::fflush(stdout);
}

//----------------------------------------------------------------------
void FTermFreeBSD::resetBeep()
{
  if ( ! FTerm::isFreeBSDTerm() )
    return;

  // Default frequency: 1491 Hz
  // Default duration:  50 ms
  FTerm::putstring (CSI "=800;5B");
  std::fflush(stdout);
}

//----------------------------------------------------------------------
void FTermFreeBSD::init()
{
  // Initialize BSD console

  fsystem = FTerm::getFSystem();
  fterm_data = FTerm::getFTermData();

  if ( ! isFreeBSDConsole() )
    return;

  if ( meta_sends_escape )
  {
    // Save current left alt key mapping
    saveFreeBSDAltKey();

    // Map meta key to left alt key
    setFreeBSDAlt2Meta();
  }

  if ( change_cursorstyle )
  {
    // Initialize FreeBSD console cursor
    setCursorStyle (fc::destructive_cursor);
  }
}

//----------------------------------------------------------------------
void FTermFreeBSD::initCharMap()
{
  // A FreeBSD console can't show ASCII codes from 0x00 to 0x1b

  if ( ! isFreeBSDConsole() )
    return;

  for (std::size_t i{0}; i <= fc::lastCharItem; i++)
    if ( fc::character[i][fc::PC] < 0x1c )
      fc::character[i][fc::PC] = fc::character[i][fc::ASCII];
}

//----------------------------------------------------------------------
void FTermFreeBSD::finish()
{
  // Resetting the FreeBSD console settings

  if ( ! isFreeBSDConsole() )
    return;

  if ( meta_sends_escape )
    resetFreeBSDAlt2Meta();

  setFreeBSDCursorStyle (fc::normal_cursor);
}


// private methods of FTermFreeBSD
//----------------------------------------------------------------------
void FTermFreeBSD::warnNotInitialized()
{
  *FApplication::getLog() << FLog::Warn
                          << "The FTermFreeBSD object has "
                          << "not yet been initialized! "
                          << "Please call the init() method first."
                          << std::endl;
}

//----------------------------------------------------------------------
bool FTermFreeBSD::saveFreeBSDAltKey()
{
  // Saving the current mapping for the alt key

  static constexpr int left_alt = 0x38;
  int ret{-1};
  keymap_t keymap{};
  initCheck(false);
  ret = fsystem->ioctl (0, GIO_KEYMAP, &keymap);

  if ( ret < 0 )
    return false;

  // Save current mapping
  bsd_alt_keymap = uInt(keymap.key[left_alt].map[0]);
  return true;
}

//----------------------------------------------------------------------
bool FTermFreeBSD::setFreeBSDAltKey (uInt key)
{
  // Remapping the alt key

  static constexpr int left_alt = 0x38;
  int ret{-1};
  keymap_t keymap{};
  initCheck(false);
  ret = fsystem->ioctl (0, GIO_KEYMAP, &keymap);

  if ( ret < 0 )
    return false;

  // Mapping "key" on the left alt key
  keymap.key[left_alt].map[0] = int(key);

  if ( (keymap.n_keys > 0)
    && fsystem && (fsystem->ioctl(0, PIO_KEYMAP, &keymap) < 0) )
    return false;
  else
    return true;
}

//----------------------------------------------------------------------
bool FTermFreeBSD::setFreeBSDAlt2Meta()
{
  // Use the meta key when pressing the Alt key

  return setFreeBSDAltKey (META);
}

//----------------------------------------------------------------------
bool FTermFreeBSD::resetFreeBSDAlt2Meta()
{
  // Restore the alt key mapping

  return setFreeBSDAltKey (bsd_alt_keymap);
}

//----------------------------------------------------------------------
bool FTermFreeBSD::setFreeBSDCursorStyle (CursorStyle style)
{
  initCheck(false);

  if ( fsystem->ioctl(0, CONS_CURSORTYPE, &style) == 0 )
    return true;
  else
    return false;
}
#endif  // defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)

}  // namespace finalcut
