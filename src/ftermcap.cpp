/***********************************************************************
* ftermcap.cpp - Provides access to terminal capabilities              *
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

#include <algorithm>
#include <string>
#include <vector>

#include "final/emptyfstring.h"
#include "final/fc.h"
#include "final/fkey_map.h"
#include "final/flog.h"
#include "final/fsystem.h"
#include "final/fterm.h"
#include "final/ftermdata.h"
#include "final/ftermcap.h"
#include "final/ftermdetection.h"

namespace finalcut
{

// static class attributes
bool             FTermcap::background_color_erase   {false};
bool             FTermcap::can_change_color_palette {false};
bool             FTermcap::automatic_left_margin    {false};
bool             FTermcap::automatic_right_margin   {false};
bool             FTermcap::eat_nl_glitch            {false};
bool             FTermcap::has_ansi_escape_sequences{false};
bool             FTermcap::ansi_default_color       {false};
bool             FTermcap::osc_support              {false};
bool             FTermcap::no_utf8_acs_chars        {false};
int              FTermcap::max_color                {1};
int              FTermcap::tabstop                  {8};
int              FTermcap::attr_without_color       {0};
FSystem*         FTermcap::fsystem                  {nullptr};
FTermData*       FTermcap::fterm_data               {nullptr};
FTermDetection*  FTermcap::term_detection           {nullptr};
char             FTermcap::string_buf[2048]         {};

//----------------------------------------------------------------------
// class FTermcap
//----------------------------------------------------------------------

/* Terminal capability data base
 * -----------------------------
 * Info under: man 5 terminfo
 *
 * Importent shell commands:
 *   captoinfo - convert all termcap descriptions into terminfo descriptions
 *   infocmp   - print out terminfo description from the current terminal
 */

// public methods of FTermcap
//----------------------------------------------------------------------
void FTermcap::init()
{
  fsystem = FTerm::getFSystem();
  fterm_data = FTerm::getFTermData();
  term_detection = FTerm::getFTermDetection();
  termcap();
}

// private methods of FTermcap
//----------------------------------------------------------------------
void FTermcap::termcap()
{
  std::vector<std::string> terminals{};
  static constexpr int success = 1;
  static constexpr int uninitialized = -2;
  static char term_buffer[BUF_SIZE]{};
  int status = uninitialized;
  const bool color256 = term_detection->canDisplay256Colors();

  // Open termcap file
#if defined(__sun) && defined(__SVR4)
  char* termtype = const_cast<char*>(fterm_data->getTermType());
#else
  const char* termtype = fterm_data->getTermType();
#endif
  terminals.push_back(termtype);            // available terminal type

  if ( color256 )                           // 1st fallback if not found
    terminals.push_back("xterm-256color");

  terminals.push_back("xterm");             // 2nd fallback if not found
  terminals.push_back("ansi");              // 3rd fallback if not found
  terminals.push_back("vt100");             // 4th fallback if not found
  auto iter = terminals.begin();

  while ( iter != terminals.end() )
  {
    fterm_data->setTermType(iter->c_str());

    // Open the termcap file + load entry for termtype
    status = tgetent(term_buffer, termtype);

    if ( status == success || ! term_detection->hasTerminalDetection() )
      break;

    ++iter;
  }

  termcapError (status);
  termcapVariables();
}

//----------------------------------------------------------------------
void FTermcap::termcapError (int status)
{
  static constexpr int no_entry = 0;
  static constexpr int db_not_found = -1;
  static constexpr int uninitialized = -2;

  if ( status == no_entry || status == uninitialized )
  {
    const char* termtype = fterm_data->getTermType();
    std::clog << FLog::Error
              << "Unknown terminal: \""  << termtype << "\". "
              << "Check the TERM environment variable. "
              << "Also make sure that the terminal "
              << "is defined in the termcap/terminfo database."
              << std::endl;
    std::abort();
  }
  else if ( status == db_not_found )
  {
    std::clog << "The termcap/terminfo database could not be found."
              << std::endl;
    std::abort();
  }
}

//----------------------------------------------------------------------
void FTermcap::termcapVariables()
{
  // Get termcap booleans
  termcapBoleans();

  // Get termcap numerics
  termcapNumerics();

  // Get termcap strings
  termcapStrings();

  // Get termcap keys
  termcapKeys();
}

//----------------------------------------------------------------------
void FTermcap::termcapBoleans()
{
  // Get termcap flags/booleans

  // Screen erased with the background color
  background_color_erase = getFlag("ut");

  // Terminal is able to redefine existing colors
  can_change_color_palette = getFlag("cc");

  // t_cursor_left wraps from column 0 to last column
  automatic_left_margin = getFlag("bw");

  // Terminal has auto-matic margins
  automatic_right_margin = getFlag("am");

  // NewLine ignored after 80 cols
  eat_nl_glitch = getFlag("xn");

  // Terminal supports ANSI set default fg and bg color
  ansi_default_color = getFlag("AX");

  // Terminal supports operating system commands (OSC)
  // OSC = Esc + ']'
  osc_support = getFlag("XT");

  // U8 is nonzero for terminals with no VT100 line-drawing in UTF-8 mode
  no_utf8_acs_chars = bool(getNumber("U8") != 0);
}

//----------------------------------------------------------------------
void FTermcap::termcapNumerics()
{
  // Get termcap numerics

  // Maximum number of colors on screen
  max_color = std::max(max_color, getNumber("Co"));

  if ( max_color < 0 )
    max_color = 1;

  if ( max_color < 8 )
    fterm_data->setMonochron(true);
  else
    fterm_data->setMonochron(false);

  // Get initial spacing for hardware tab stop
  tabstop = getNumber("it");

  // Get video attributes that cannot be used with colors
  attr_without_color = getNumber("NC");
}

//----------------------------------------------------------------------
void FTermcap::termcapStrings()
{
  // Get termcap strings

  // Read termcap output strings

  for (auto&& entry : strings)
    entry.string = getString(entry.tname);

  const auto& ho = TCAP(fc::t_cursor_home);

  if ( std::strncmp(ho, "\033[H", 3) == 0 )
    has_ansi_escape_sequences = true;
}

//----------------------------------------------------------------------
void FTermcap::termcapKeys()
{
  // Get termcap keys

  // Read termcap key sequences up to the self-defined values
  for (auto&& entry : fc::fkey)
  {
    if ( entry.string != nullptr )
      break;

    entry.string = getString(entry.tname);
  }
}

//----------------------------------------------------------------------
int FTermcap::_tputs (const char* str, int affcnt, fn_putc putc)
{
  if ( ! fsystem )
    fsystem = FTerm::getFSystem();

  return fsystem->tputs (str, affcnt, putc);
}


// private Data Member of FTermcap - termcap capabilities
//----------------------------------------------------------------------
FTermcap::TCapMapType FTermcap::strings =
{{
//  .------------- term string
//  |    .-------- Tcap-code
//  |    |      // variable name                -> description
//------------------------------------------------------------------------------
  { nullptr, "bl" },  // bell                   -> audible signal (bell) (P)
  { nullptr, "ec" },  // erase_chars            -> erase #1 characters (P)
  { nullptr, "cl" },  // clear_screen           -> clear screen and home cursor (P*)
  { nullptr, "cd" },  // clr_eos                -> clear to end of screen (P*)
  { nullptr, "ce" },  // clr_eol                -> clear to end of line (P)
  { nullptr, "cb" },  // clr_bol                -> Clear to beginning of line
  { nullptr, "ho" },  // cursor_home            -> home cursor (if no cup)
  { nullptr, "ll" },  // cursor_to_ll           -> last line, first column (if no cup)
  { nullptr, "cr" },  // carriage_return        -> carriage return (P*)
  { nullptr, "ta" },  // tab                    -> tab to next 8-space hardware tab stop
  { nullptr, "bt" },  // back_tab               -> back tab (P)
  { nullptr, "ip" },  // insert_padding         -> insert padding after inserted character
  { nullptr, "ic" },  // insert_character       -> insert character (P)
  { nullptr, "IC" },  // parm_ich               -> insert #1 characters (P*)
  { nullptr, "rp" },  // repeat_char            -> repeat char #1 #2 times (P*)
  { nullptr, "Ic" },  // initialize_color       -> initialize color #1 to (#2,#3,#4)
  { nullptr, "Ip" },  // initialize_pair        -> Initialize color pair #1 to
                      //                           fg=(#2,#3,#4), bg=(#5,#6,#7)
  { nullptr, "AF" },  // set_a_foreground       -> Set ANSI background color to #1
  { nullptr, "AB" },  // set_a_background       -> Set ANSI background color to #1
  { nullptr, "Sf" },  // set_foreground         -> Set foreground color #1
  { nullptr, "Sb" },  // set_background         -> Set background color #1
  { nullptr, "sp" },  // set_color_pair         -> Set current color pair to #1
  { nullptr, "op" },  // orig_pair              -> Set default pair to original value
  { nullptr, "oc" },  // orig_colors            -> Set all color pairs to the original
  { nullptr, "NC" },  // no_color_video         -> video attributes that cannot be used
                      //                           with colors
  { nullptr, "cm" },  // cursor_address         -> move to row #1 columns #2
  { nullptr, "ch" },  // column_address         -> horizontal position #1, absolute (P)
  { nullptr, "cv" },  // row_address            -> vertical position #1 absolute (P)
  { nullptr, "vs" },  // cursor_visible         -> make cursor very visible
  { nullptr, "vi" },  // cursor_invisible       -> make cursor invisible
  { nullptr, "ve" },  // cursor_normal          -> make cursor appear normal (undo vi/vs)
  { nullptr, "up" },  // cursor_up              -> up one line
  { nullptr, "do" },  // cursor_down            -> down one line
  { nullptr, "le" },  // cursor_left            -> move left one space
  { nullptr, "nd" },  // cursor_right           -> non-destructive space (move right)
  { nullptr, "UP" },  // parm_up_cursor         -> up #1 lines (P*)
  { nullptr, "DO" },  // parm_down_cursor       -> down #1 lines (P*)
  { nullptr, "LE" },  // parm_left_cursor       -> move #1 characters to the left (P)
  { nullptr, "RI" },  // parm_right_cursor      -> move #1 characters to the right (P*)
  { nullptr, "sc" },  // save_cursor            -> save current cursor position (P)
  { nullptr, "rc" },  // restore_cursor         -> restore cursor to save_cursor
  { nullptr, "Ss" },  // set cursor style       -> Select the DECSCUSR cursor style
  { nullptr, "sf" },  // scroll_forward         -> scroll text up (P)
  { nullptr, "sr" },  // scroll_reverse         -> scroll text down (P)
  { nullptr, "ti" },  // enter_ca_mode          -> string to start programs using cup
  { nullptr, "te" },  // exit_ca_mode           -> strings to end programs using cup
  { nullptr, "eA" },  // enable_acs             -> enable alternate char set
  { nullptr, "md" },  // enter_bold_mode        -> turn on bold (double-bright) mode
  { nullptr, "me" },  // exit_bold_mode         -> turn off bold mode
  { nullptr, "mh" },  // enter_dim_mode         -> turn on half-bright
  { nullptr, "me" },  // exit_dim_mode          -> turn off half-bright
  { nullptr, "ZH" },  // enter_italics_mode     -> Enter italic mode
  { nullptr, "ZR" },  // exit_italics_mode      -> End italic mode
  { nullptr, "us" },  // enter_underline_mode   -> begin underline mode
  { nullptr, "ue" },  // exit_underline_mode    -> exit underline mode
  { nullptr, "mb" },  // enter_blink_mode       -> turn on blinking
  { nullptr, "me" },  // exit_blink_mode        -> turn off blinking
  { nullptr, "mr" },  // enter_reverse_mode     -> turn on reverse video mode
  { nullptr, "me" },  // exit_reverse_mode      -> turn off reverse video mode
  { nullptr, "so" },  // enter_standout_mode    -> begin standout mode
  { nullptr, "se" },  // exit_standout_mode     -> exit standout mode
  { nullptr, "mk" },  // enter_secure_mode      -> turn on blank mode (characters invisible)
  { nullptr, "me" },  // exit_secure_mode       -> turn off blank mode (characters visible)
  { nullptr, "mp" },  // enter_protected_mode   -> turn on protected mode
  { nullptr, "me" },  // exit_protected_mode    -> turn off protected mode
  { nullptr, "XX" },  // enter_crossed_out_mode -> turn on mark character as deleted
  { nullptr, "me" },  // exit_crossed_out_mode  -> turn off mark character as deleted
  { nullptr, "Us" },  // enter_dbl_underline_mode -> begin double underline mode
  { nullptr, "Ue" },  // exit_dbl_underline_mode  -> exit double underline mode
  { nullptr, "sa" },  // set_attributes         -> define videoattributes #1-#9 (PG9)
  { nullptr, "me" },  // exit_attribute_mode    -> turn off all attributes
  { nullptr, "as" },  // enter_alt_charset_mode -> start alternate character set (P)
  { nullptr, "ae" },  // exit_alt_charset_mode  -> end alternate character set (P)
  { nullptr, "S2" },  // enter_pc_charset_mode  -> Enter PC character display mode
  { nullptr, "S3" },  // exit_pc_charset_mode   -> Exit PC character display mode
  { nullptr, "im" },  // enter_insert_mode      -> enter insert mode
  { nullptr, "ei" },  // exit_insert_mode       -> exit insert mode
  { nullptr, "SA" },  // enter_am_mode          -> turn on automatic margins
  { nullptr, "RA" },  // exit_am_mode           -> turn off automatic margins
  { nullptr, "ac" },  // acs_chars              -> graphics charset pairs (vt100)
  { nullptr, "ks" },  // keypad_xmit            -> enter 'key-board_transmit' mode
  { nullptr, "ke" },  // keypad_local           -> leave 'key-board_transmit' mode
  { nullptr, "Km" }   // key_mouse              -> Mouse event has occurred
}};

/*
 * (P)    indicates that padding may be specified
 * #[1-9] in the description field indicates that the string
 *        is passed through tparm with parms as given (#i).
 * (P*)   indicates that padding may vary in proportion
 *        to the number of lines affected
 * (#i)   indicates the ith parameter.
 *
 * "XX", "Us" and "Ue" are unofficial and they are only used here.
 */

}  // namespace finalcut
