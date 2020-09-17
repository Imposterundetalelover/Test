/***********************************************************************
* fkeyboard.cpp - Read keyboard events                                 *
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

#include <fcntl.h>

#if defined(__CYGWIN__)
  #include <sys/select.h>  // need for FD_ZERO, FD_SET, FD_CLR, ...
#endif

#include <string>

#include "final/fkeyboard.h"
#include "final/fkey_map.h"
#include "final/fobject.h"
#include "final/fterm.h"
#include "final/ftermios.h"

#if defined(__linux__)
  #include "final/ftermlinux.h"
#endif

namespace finalcut
{

// static class attributes
uInt64 FKeyboard::read_blocking_time{100000};  // preset to 100 ms / 10 Hz
uInt64 FKeyboard::key_timeout{100000};         // preset to 100 ms / 10 Hz
uInt64 FKeyboard::interval_timeout{75000};     // preset to 75 ms / 13.3 Hz
struct timeval FKeyboard::time_keypressed{};
struct timeval FKeyboard::time_last_request{};

#if defined(__linux__)
  FTermLinux* FKeyboard::linux{nullptr};
#endif


//----------------------------------------------------------------------
// class FKeyboard
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FKeyboard::FKeyboard()
{
  // Initialize keyboard values
  time_keypressed.tv_sec = 0;
  time_keypressed.tv_usec = 0;
  time_last_request.tv_sec = 0;
  time_last_request.tv_usec = 0;

  // Get the stdin file status flags
  stdin_status_flags = fcntl(FTermios::getStdIn(), F_GETFL);

  if ( stdin_status_flags == -1 )
    std::abort();
}

//----------------------------------------------------------------------
FKeyboard::~FKeyboard()  // destructor
{ }

// public methods of FKeyboard
//----------------------------------------------------------------------
void FKeyboard::fetchKeyCode()
{
  parseKeyBuffer();
}

//----------------------------------------------------------------------
const FString FKeyboard::getKeyName (const FKey keynum) const
{
  for (std::size_t i{0}; fc::fkeyname[i].string[0] != 0; i++)
    if ( fc::fkeyname[i].num && fc::fkeyname[i].num == keynum )
      return FString{fc::fkeyname[i].string};

  if ( keynum > 32 && keynum < 127 )
    return FString{char(keynum)};

  return FString{""};
}

//----------------------------------------------------------------------
void FKeyboard::setTermcapMap (fc::FKeyMap* keymap)
{
  key_map = keymap;
}

//----------------------------------------------------------------------
void FKeyboard::init()
{
#if defined(__linux__)
  linux = FTerm::getFTermLinux();
#endif
}

//----------------------------------------------------------------------
bool& FKeyboard::unprocessedInput()
{
  return input_data_pending;
}

//----------------------------------------------------------------------
bool FKeyboard::isKeyPressed() const
{
  if ( ! isIntervalTimeout() )
    return false;

  fd_set ifds{};
  struct timeval tv{};
  const int stdin_no = FTermios::getStdIn();

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = suseconds_t(read_blocking_time);  // preset to 100 ms
  FObject::getCurrentTime (&time_last_request);
  const int result = select (stdin_no + 1, &ifds, nullptr, nullptr, &tv);

  if ( result > 0 && FD_ISSET(stdin_no, &ifds) )
    FD_CLR (stdin_no, &ifds);

  return ( result > 0 );
}

//----------------------------------------------------------------------
void FKeyboard::clearKeyBuffer()
{
  // Empty the buffer

  fifo_offset = 0;
  key = 0;
  std::fill_n (fifo_buf, FIFO_BUF_SIZE, '\0');
  fifo_in_use = false;
}

//----------------------------------------------------------------------
void FKeyboard::clearKeyBufferOnTimeout()
{
  // Empty the buffer on timeout

  if ( fifo_in_use && isKeypressTimeout() )
    clearKeyBuffer();
}

//----------------------------------------------------------------------
void FKeyboard::escapeKeyHandling()
{
  // Send an escape key press event if there is only one 0x1b
  // in the buffer and the timeout is reached

  if ( fifo_in_use
    && fifo_offset == 1
    && fifo_buf[0] == 0x1b
    && fifo_buf[1] == 0x00
    && isKeypressTimeout() )
  {
    fifo_offset = 0;
    fifo_buf[0] = 0x00;
    fifo_in_use = false;
    input_data_pending = false;
    escapeKeyPressed();
  }

  // Handling of keys that are substrings of other keys
  substringKeyHandling();
}


// private methods of FKeyboard
//----------------------------------------------------------------------
inline FKey FKeyboard::getMouseProtocolKey() const
{
  // Looking for mouse string in the key buffer

  if ( ! mouse_support )
    return NOT_SET;

  const std::size_t buf_len = std::strlen(fifo_buf);

  // x11 mouse tracking
  if ( buf_len >= 6 && fifo_buf[1] == '[' && fifo_buf[2] == 'M' )
    return fc::Fkey_mouse;

  // SGR mouse tracking
  if ( fifo_buf[1] == '[' && fifo_buf[2] == '<' && buf_len >= 9
    && (fifo_buf[buf_len - 1] == 'M' || fifo_buf[buf_len - 1] == 'm') )
    return fc::Fkey_extended_mouse;

  // urxvt mouse tracking
  if ( fifo_buf[1] == '[' && fifo_buf[2] >= '1' && fifo_buf[2] <= '9'
    && fifo_buf[3] >= '0' && fifo_buf[3] <= '9' && buf_len >= 9
    && fifo_buf[buf_len - 1] == 'M' )
    return fc::Fkey_urxvt_mouse;

  return NOT_SET;
}

//----------------------------------------------------------------------
inline FKey FKeyboard::getTermcapKey()
{
  // Looking for termcap key strings in the buffer

  assert ( FIFO_BUF_SIZE > 0 );

  if ( ! key_map )
    return NOT_SET;

  for (std::size_t i{0}; key_map[i].tname[0] != 0; i++)
  {
    const char* k = key_map[i].string;
    const std::size_t len = ( k ) ? std::strlen(k) : 0;

    if ( k && std::strncmp(k, fifo_buf, len) == 0 )  // found
    {
      std::size_t n{};

      for (n = len; n < FIFO_BUF_SIZE; n++)  // Remove founded entry
        fifo_buf[n - len] = fifo_buf[n];

      for (n = n - len; n < FIFO_BUF_SIZE; n++)  // Fill rest with '\0'
        fifo_buf[n] = '\0';

      input_data_pending = bool(fifo_buf[0] != '\0');
      return fc::fkey[i].num;
    }
  }

  return NOT_SET;
}

//----------------------------------------------------------------------
inline FKey FKeyboard::getMetaKey()
{
  // Looking for meta key strings in the buffer

  assert ( FIFO_BUF_SIZE > 0 );

  for (std::size_t i{0}; fc::fmetakey[i].string[0] != 0; i++)
  {
    const char* kmeta = fc::fmetakey[i].string;  // The string is never null
    const std::size_t len = std::strlen(kmeta);

    if ( std::strncmp(kmeta, fifo_buf, len) == 0 )  // found
    {
      std::size_t n{};

      if ( len == 2
        && ( fifo_buf[1] == 'O'
          || fifo_buf[1] == '['
          || fifo_buf[1] == ']' )
        && ! isKeypressTimeout() )
      {
        return fc::Fkey_incomplete;
      }

      for (n = len; n < FIFO_BUF_SIZE; n++)  // Remove founded entry
        fifo_buf[n - len] = fifo_buf[n];

      for (n = n - len; n < FIFO_BUF_SIZE; n++)  // Fill rest with '\0'
        fifo_buf[n] = '\0';

      input_data_pending = bool(fifo_buf[0] != '\0');
      return fc::fmetakey[i].num;
    }
  }

  return NOT_SET;
}

//----------------------------------------------------------------------
inline FKey FKeyboard::getSingleKey()
{
  // Looking for single key code in the buffer

  std::size_t n{};
  std::size_t len{1};
  const uChar firstchar = uChar(fifo_buf[0]);
  FKey keycode{};

  // Look for a utf-8 character
  if ( utf8_input && (firstchar & 0xc0) == 0xc0 )
  {
    char utf8char[5]{};  // Init array with '\0'
    const std::size_t buf_len = std::strlen(fifo_buf);

    if ( (firstchar & 0xe0) == 0xc0 )
      len = 2;
    else if ( (firstchar & 0xf0) == 0xe0 )
      len = 3;
    else if ( (firstchar & 0xf8) == 0xf0 )
      len = 4;

    if ( buf_len <  len && ! isKeypressTimeout() )
      return fc::Fkey_incomplete;

    for (std::size_t i{0}; i < len ; i++)
      utf8char[i] = char(fifo_buf[i] & 0xff);

    keycode = UTF8decode(utf8char);
  }
  else
    keycode = uChar(fifo_buf[0] & 0xff);

  for (n = len; n < FIFO_BUF_SIZE; n++)  // Remove the key from the buffer front
    fifo_buf[n - len] = fifo_buf[n];

  for (n = n - len; n < FIFO_BUF_SIZE; n++)  // Fill the rest with '\0' bytes
    fifo_buf[n] = '\0';

  input_data_pending = bool(fifo_buf[0] != '\0');

  if ( keycode == 0 )  // Ctrl+Space or Ctrl+@
    keycode = fc::Fckey_space;

  return FKey(keycode == 127 ? fc::Fkey_backspace : keycode);
}

//----------------------------------------------------------------------
bool FKeyboard::setNonBlockingInput (bool enable)
{
  if ( enable == non_blocking_stdin )
    return non_blocking_stdin;

  if ( enable )  // make stdin non-blocking
  {
    stdin_status_flags |= O_NONBLOCK;

    if ( fcntl (FTermios::getStdIn(), F_SETFL, stdin_status_flags) != -1 )
      non_blocking_stdin = true;
  }
  else
  {
    stdin_status_flags &= ~O_NONBLOCK;

    if ( fcntl (FTermios::getStdIn(), F_SETFL, stdin_status_flags) != -1 )
      non_blocking_stdin = false;
  }

  return non_blocking_stdin;
}

//----------------------------------------------------------------------
inline bool FKeyboard::isKeypressTimeout()
{
  return FObject::isTimeout (&time_keypressed, key_timeout);
}

//----------------------------------------------------------------------
inline bool FKeyboard::isIntervalTimeout()
{
  return FObject::isTimeout (&time_last_request, interval_timeout);
}

//----------------------------------------------------------------------
FKey FKeyboard::UTF8decode (const char utf8[]) const
{
  FKey ucs{0};  // Universal coded character
  constexpr std::size_t max = 4;
  std::size_t len = std::strlen(utf8);

  if ( len > max )
    len = max;

  for (std::size_t i{0}; i < len; ++i)
  {
    const uChar ch = uChar(utf8[i]);

    if ( (ch & 0xc0) == 0x80 )
    {
      // byte 2..4 = 10xxxxxx
      ucs = (ucs << 6) | (ch & 0x3f);
    }
    else if ( ch < 128 )
    {
      // byte 1 = 0xxxxxxx (1 byte mapping)
      ucs = ch & 0xff;
    }
    else if ( (ch & 0xe0) == 0xc0 )
    {
      // byte 1 = 110xxxxx (2 byte mapping)
      ucs = ch & 0x1f;
    }
    else if ( (ch & 0xf0) == 0xe0 )
    {
      // byte 1 = 1110xxxx (3 byte mapping)
      ucs = ch & 0x0f;
    }
    else if ( (ch & 0xf8) == 0xf0 )
    {
      // byte 1 = 11110xxx (4 byte mapping)
      ucs = ch & 0x07;
    }
    else
    {
      // error
      ucs = NOT_SET;
    }
  }

  return ucs;
}

//----------------------------------------------------------------------
inline ssize_t FKeyboard::readKey()
{
  setNonBlockingInput();
  const ssize_t bytes = read(FTermios::getStdIn(), &read_character, 1);
  unsetNonBlockingInput();
  return bytes;
}

//----------------------------------------------------------------------
void FKeyboard::parseKeyBuffer()
{
  ssize_t bytesread{};
  FObject::getCurrentTime (&time_keypressed);

  while ( (bytesread = readKey()) > 0 )
  {
    if ( bytesread + fifo_offset <= int(FIFO_BUF_SIZE) )
    {
      fifo_buf[fifo_offset] = char(read_character);
      fifo_offset++;
      fifo_in_use = true;
    }

    // Read the rest from the fifo buffer
    while ( ! isKeypressTimeout()
         && fifo_offset > 0
         && key != fc::Fkey_incomplete )
    {
      key = parseKeyString();
      key = keyCorrection(key);

      if ( key != fc::Fkey_incomplete )
        keyPressed();

      fifo_offset = int(std::strlen(fifo_buf));

      if ( key == fc::Fkey_mouse
        || key == fc::Fkey_extended_mouse
        || key == fc::Fkey_urxvt_mouse )
        break;
    }

    // Send key up event
    if ( key > 0 )
      keyReleased();

    key = 0;
  }

  read_character = 0;
}

//----------------------------------------------------------------------
FKey FKeyboard::parseKeyString()
{
  const uChar firstchar = uChar(fifo_buf[0]);

  if ( firstchar == ESC[0] )
  {
    FKey keycode = getMouseProtocolKey();

    if ( keycode != NOT_SET )
      return keycode;

    keycode = getTermcapKey();

    if ( keycode != NOT_SET )
      return keycode;

    keycode = getMetaKey();

    if ( keycode != NOT_SET )
      return keycode;

    if ( ! isKeypressTimeout() )
      return fc::Fkey_incomplete;
  }

  return getSingleKey();
}

//----------------------------------------------------------------------
FKey FKeyboard::keyCorrection (const FKey& keycode) const
{
  FKey key_correction;

#if defined(__linux__)
  if ( linux )
    key_correction = linux->modifierKeyCorrection(keycode);
  else
    key_correction = keycode;
#else
  key_correction = keycode;
#endif

  return key_correction;
}

//----------------------------------------------------------------------
void FKeyboard::substringKeyHandling()
{
  // Some keys (Meta-O, Meta-[, Meta-]) used substrings
  // of other keys and are only processed after a timeout

  if ( fifo_in_use
    && fifo_offset == 2
    && fifo_buf[0] == 0x1b
    && (fifo_buf[1] == 'O' || fifo_buf[1] == '[' || fifo_buf[1] == ']')
    && fifo_buf[2] == '\0'
    && isKeypressTimeout() )
  {
    fifo_offset = 0;
    fifo_buf[0] = 0x00;
    fifo_in_use = false;
    input_data_pending = false;

    if ( fifo_buf[1] == 'O' )
      key = fc::Fmkey_O;
    else if ( fifo_buf[1] == '[' )
      key = fc::Fmkey_left_square_bracket;
    else
      key = fc::Fmkey_right_square_bracket;

    keyPressed();
    keyReleased();
  }
}

//----------------------------------------------------------------------
void FKeyboard::keyPressed() const
{
  keypressed_cmd.execute();
}

//----------------------------------------------------------------------
void FKeyboard::keyReleased() const
{
  keyreleased_cmd.execute();
}

//----------------------------------------------------------------------
void FKeyboard::escapeKeyPressed() const
{
  escape_key_cmd.execute();
}

}  // namespace finalcut
