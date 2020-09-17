/***********************************************************************
* termcap.cpp - Show the used termcap variables                        *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2020 Markus Gans                                      *
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

#include <iomanip>
#include <iostream>
#include <string>

#include <final/final.h>

namespace fc = finalcut::fc;

// Function prototype
void tcapBoolean (const std::string&, bool);
void tcapNumeric (const std::string&, int);
void tcapString (const std::string&, const char[]);
void debug (const finalcut::FApplication&);
void booleans();
void numeric();
void string();


//----------------------------------------------------------------------
// struct data
//----------------------------------------------------------------------

struct data
{
  struct alignas(alignof(std::string)) termcap_string
  {
    const std::string name;
    const fc::termcaps cap;
  };

  static termcap_string strings[];
};

//----------------------------------------------------------------------
// struct data - string data array
//----------------------------------------------------------------------
data::termcap_string data::strings[] =
{
  { "t_bell", fc::t_bell },
  { "t_erase_chars", fc::t_erase_chars },
  { "t_clear_screen", fc::t_clear_screen },
  { "t_clr_eos", fc::t_clr_eos },
  { "t_clr_eol", fc::t_clr_eol },
  { "t_clr_bol", fc::t_clr_bol },
  { "t_cursor_home", fc::t_cursor_home },
  { "t_cursor_to_ll", fc::t_cursor_to_ll },
  { "t_carriage_return", fc::t_carriage_return },
  { "t_tab", fc::t_tab },
  { "t_back_tab", fc::t_back_tab },
  { "t_insert_padding", fc::t_insert_padding },
  { "t_insert_character", fc::t_insert_character },
  { "t_parm_ich", fc::t_parm_ich },
  { "t_repeat_char", fc::t_repeat_char },
  { "t_initialize_color", fc::t_initialize_color },
  { "t_initialize_pair", fc::t_initialize_pair },
  { "t_set_a_foreground", fc::t_set_a_foreground },
  { "t_set_a_background", fc::t_set_a_background },
  { "t_set_foreground", fc::t_set_foreground },
  { "t_set_background", fc::t_set_background },
  { "t_set_color_pair", fc::t_set_color_pair },
  { "t_orig_pair", fc::t_orig_pair },
  { "t_orig_colors", fc::t_orig_colors },
  { "t_no_color_video", fc::t_no_color_video },
  { "t_cursor_address", fc::t_cursor_address },
  { "t_column_address", fc::t_column_address },
  { "t_row_address", fc::t_row_address },
  { "t_cursor_visible", fc::t_cursor_visible },
  { "t_cursor_invisible", fc::t_cursor_invisible },
  { "t_cursor_normal", fc::t_cursor_normal },
  { "t_cursor_up", fc::t_cursor_up },
  { "t_cursor_down", fc::t_cursor_down },
  { "t_cursor_left", fc::t_cursor_left },
  { "t_cursor_right", fc::t_cursor_right },
  { "t_parm_up_cursor", fc::t_parm_up_cursor },
  { "t_parm_down_cursor", fc::t_parm_down_cursor },
  { "t_parm_left_cursor", fc::t_parm_left_cursor },
  { "t_parm_right_cursor", fc::t_parm_right_cursor },
  { "t_save_cursor", fc::t_save_cursor },
  { "t_restore_cursor", fc::t_restore_cursor },
  { "t_scroll_forward", fc::t_scroll_forward },
  { "t_scroll_reverse", fc::t_scroll_reverse },
  { "t_enter_ca_mode", fc::t_enter_ca_mode },
  { "t_exit_ca_mode", fc::t_exit_ca_mode },
  { "t_enable_acs", fc::t_enable_acs },
  { "t_enter_bold_mode", fc::t_enter_bold_mode },
  { "t_exit_bold_mode", fc::t_exit_bold_mode },
  { "t_enter_dim_mode", fc::t_enter_dim_mode },
  { "t_exit_dim_mode", fc::t_exit_dim_mode },
  { "t_enter_italics_mode", fc::t_enter_italics_mode },
  { "t_exit_italics_mode", fc::t_exit_italics_mode },
  { "t_enter_underline_mode", fc::t_enter_underline_mode },
  { "t_exit_underline_mode", fc::t_exit_underline_mode },
  { "t_enter_blink_mode", fc::t_enter_blink_mode },
  { "t_exit_blink_mode", fc::t_exit_blink_mode },
  { "t_enter_reverse_mode", fc::t_enter_reverse_mode },
  { "t_exit_reverse_mode", fc::t_exit_reverse_mode },
  { "t_enter_standout_mode", fc::t_enter_standout_mode },
  { "t_exit_standout_mode", fc::t_exit_standout_mode },
  { "t_enter_secure_mode", fc::t_enter_secure_mode },
  { "t_exit_secure_mode", fc::t_exit_secure_mode },
  { "t_enter_protected_mode", fc::t_enter_protected_mode },
  { "t_exit_protected_mode", fc::t_exit_protected_mode },
  { "t_enter_crossed_out_mode", fc::t_enter_crossed_out_mode },
  { "t_exit_crossed_out_mode", fc::t_exit_crossed_out_mode },
  { "t_enter_dbl_underline_mode", fc::t_enter_dbl_underline_mode },
  { "t_exit_dbl_underline_mode", fc::t_exit_dbl_underline_mode },
  { "t_set_attributes", fc::t_set_attributes },
  { "t_exit_attribute_mode", fc::t_exit_attribute_mode },
  { "t_enter_alt_charset_mode", fc::t_enter_alt_charset_mode },
  { "t_exit_alt_charset_mode", fc::t_exit_alt_charset_mode },
  { "t_enter_pc_charset_mode", fc::t_enter_pc_charset_mode },
  { "t_exit_pc_charset_mode", fc::t_exit_pc_charset_mode },
  { "t_enter_insert_mode", fc::t_enter_insert_mode },
  { "t_exit_insert_mode", fc::t_exit_insert_mode },
  { "t_enter_am_mode", fc::t_enter_am_mode },
  { "t_exit_am_mode", fc::t_exit_am_mode },
  { "t_acs_chars", fc::t_acs_chars },
  { "t_keypad_xmit", fc::t_keypad_xmit },
  { "t_keypad_local", fc::t_keypad_local },
  { "t_key_mouse", fc::t_key_mouse }
};


//----------------------------------------------------------------------
// Functions
//----------------------------------------------------------------------
void tcapBoolean (const std::string& name, bool cap_bool)
{
  std::cout << "FTermcap::" << name << ": ";

  if ( cap_bool )
    std::cout << "true\r\n";
  else
    std::cout << "false\r\n";
}

//----------------------------------------------------------------------
void tcapNumeric (const std::string& name, int cap_num)
{
  std::cout << "FTermcap::" << name << ": " << cap_num << "\r\n";
}

//----------------------------------------------------------------------
void tcapString (const std::string& name, const char cap_str[])
{
  std::string sequence{};
  std::cout << name << ": ";

  if ( cap_str == nullptr )
  {
    std::cout << "\r\n";
    return;
  }

  const uInt len = uInt(std::strlen(cap_str));

  for (uInt i{0}; i < len; i++)
  {
    const uChar c = uChar(cap_str[i]);

    if ( c > 127 )
    {
      std::ostringstream o;
      o << std::oct << int(c);
      sequence += "\\";
      sequence += o.str();
    }
    else if ( c < 32 )
    {
      if ( c == 27 )
        sequence += "\\E";
      else
      {
        sequence += '^';
        sequence += char(c + 64);
      }
    }
    else
      sequence += char(c);
  }

  std::cout << sequence << " ";
  std::cout << "\r\n";
}

//----------------------------------------------------------------------
#if DEBUG
void debug (const finalcut::FApplication& TermApp)
{
  const auto& fterm = TermApp.getFTerm();
  auto& debug_data = fterm.getFTermDebugData();
  const finalcut::FString& ab_s = debug_data.getAnswerbackString();
  const finalcut::FString& sec_da = debug_data.getSecDAString();
  std::cout << "\n.------------------- debug -------------------\r\n";

#if defined(__linux__)
  std::cout << "|               Framebuffer bpp: "
            << debug_data.getFramebufferBpp() << "\r\n";
#endif

  std::cout << "| after init_256colorTerminal(): "
            << debug_data.getTermType_256color() << "\r\n";
  std::cout << "|    after parseAnswerbackMsg(): "
            << debug_data.getTermType_Answerback() << "\r\n";
  std::cout << "|            after parseSecDA(): "
            << debug_data.getTermType_SecDA() << "\r\n";

  if ( ! ab_s.isEmpty() )
    tcapString ("|         The answerback String", ab_s);

  if ( ! sec_da.isEmpty() )
    tcapString ("|              The SecDA String", sec_da);

  std::cout << "`------------------- debug -------------------\r\n";
}
#else
void debug (finalcut::FApplication&)
{
  // FINAL CUT was compiled without debug option
}
#endif

//----------------------------------------------------------------------
void booleans()
{
  std::cout << "\r\n[Booleans]\r\n";
  tcapBoolean ( "background_color_erase"
              , finalcut::FTermcap::background_color_erase );
  tcapBoolean ( "can_change_color_palette"
              , finalcut::FTermcap::can_change_color_palette );
  tcapBoolean ( "automatic_left_margin"
              , finalcut::FTermcap::automatic_left_margin );
  tcapBoolean ( "automatic_right_margin"
              , finalcut::FTermcap::automatic_right_margin );
  tcapBoolean ( "eat_nl_glitch"
              , finalcut::FTermcap::eat_nl_glitch );
  tcapBoolean ( "has_ansi_escape_sequences"
              , finalcut::FTermcap::has_ansi_escape_sequences );
  tcapBoolean ( "ansi_default_color"
              , finalcut::FTermcap::ansi_default_color );
  tcapBoolean ( "osc_support"
              , finalcut::FTermcap::osc_support );
  tcapBoolean ( "no_utf8_acs_chars"
              , finalcut::FTermcap::no_utf8_acs_chars );
}

//----------------------------------------------------------------------
void numeric()
{
  std::cout << "\r\n[Numeric]\r\n";
  tcapNumeric ("max_color"
              , finalcut::FTermcap::max_color);
  tcapNumeric ("tabstop"
              , finalcut::FTermcap::tabstop);
  tcapNumeric ("attr_without_color"
              , finalcut::FTermcap::attr_without_color);
}

//----------------------------------------------------------------------
void string()
{
  std::cout << "\r\n[String]\r\n";
  const finalcut::FTermcap::tcap_map (&tcap_strings)[] \
      = finalcut::FTermcap::strings;

  for (const auto& entry : data::strings)
  {
    const std::string name = entry.name;
    const fc::termcaps cap = entry.cap;
    tcapString (name, tcap_strings[cap].string);
  }
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Disabling the switch to the alternative screen
  finalcut::FTerm::useAlternateScreen(false);

  // Disable color palette changes and terminal data requests
  auto& start_options = finalcut::FStartOptions::getFStartOptions();
  start_options.color_change = false;
  start_options.terminal_data_request = false;

  // Create the application object as root widget
  finalcut::FApplication term_app {argc, argv};

  // Force terminal initialization without calling show()
  term_app.initTerminal();

  if ( finalcut::FApplication::isQuit() )
    return 0;

  std::cout << "--------\r\nFTermcap\r\n--------\r\n\n";
  std::cout << "Terminal: " << finalcut::FTerm::getTermType() << "\r\n";

  debug (term_app);

  booleans();
  numeric();
  string();
  return 0;
}
