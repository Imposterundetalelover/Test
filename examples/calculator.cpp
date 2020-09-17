/***********************************************************************
* calculator.cpp - A simple calculator with trigonometric functions    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2020 Markus Gans                                      *
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

#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <map>
#include <memory>
#include <stack>

#include <final/final.h>

namespace fc = finalcut::fc;
using finalcut::FPoint;
using finalcut::FRect;
using finalcut::FSize;
using finalcut::FColorPair;

constexpr lDouble PI{3.141592653589793238L};


//----------------------------------------------------------------------
// class Button
//----------------------------------------------------------------------

class Button final : public finalcut::FButton
{
  public:
    // Constructor
    explicit Button (FWidget* = nullptr);

    // Method
    void setChecked(bool);

    // Event handler
    void onKeyPress (finalcut::FKeyEvent*) override;

  private:
    // Data member
    bool checked{false};
};

//----------------------------------------------------------------------
Button::Button (finalcut::FWidget* parent)
  : finalcut::FButton{parent}
{ }

//----------------------------------------------------------------------
void Button::setChecked (bool enable)
{
  if ( checked == enable )
    return;

  checked = enable;

  if ( checked )
  {
    setBackgroundColor(fc::Cyan);
    setFocusForegroundColor(fc::White);
    setFocusBackgroundColor(fc::Cyan);
  }
  else
  {
    const auto& wc = getColorTheme();
    setBackgroundColor(wc->button_active_bg);
    setFocusForegroundColor(wc->button_active_focus_fg);
    setFocusBackgroundColor(wc->button_active_focus_bg);
  }

  redraw();
}

//----------------------------------------------------------------------
void Button::onKeyPress (finalcut::FKeyEvent* ev)
{
  const FKey key = ev->key();

  // catch the enter key
  if ( key == fc::Fkey_return || key == fc::Fkey_enter )
    return;

  finalcut::FButton::onKeyPress(ev);
}


//----------------------------------------------------------------------
// class Calc
//----------------------------------------------------------------------

class Calc final : public finalcut::FDialog
{
  public:
    // Using-declaration
    using FDialog::setGeometry;

    // Constructor
    explicit Calc (finalcut::FWidget* parent = nullptr);

    // Destructor
    ~Calc() override;

  private:
    // Typedef and Enumeration
    typedef std::function<void(lDouble&)> keyFunction;  // Member function

    enum button
    {
      Sine,
      Cosine,
      Tangent,
      Reciprocal,
      On,
      Natural_logarithm,
      Powers_of_e,
      Power,
      Square_root,
      Divide,
      Common_logarithm,
      Powers_of_ten,
      Parenthese_l,
      Parenthese_r,
      Multiply,
      Hyperbolic,
      Seven,
      Eight,
      Nine,
      Subtract,
      Arcus,
      Four,
      Five,
      Six,
      Add,
      Pi,
      One,
      Two,
      Three,
      Percent,
      Zero,
      Decimal_point,
      Change_sign,
      Equals,
      NUM_OF_BUTTONS
    };

    // Methods
    void           drawDispay();
    void           draw() override;
    void           sendOnButtonAccelerator();
    void           clear (const lDouble&);
    void           zero (const lDouble&);
    void           one (const lDouble&);
    void           two (const lDouble&);
    void           three (const lDouble&);
    void           four (const lDouble&);
    void           five (const lDouble&);
    void           six (const lDouble&);
    void           seven (const lDouble&);
    void           eight (const lDouble&);
    void           nine (const lDouble&);
    void           add (const lDouble&);
    void           subtract (const lDouble&);
    void           multiply (const lDouble&);
    void           divide (const lDouble&);
    void           equals (const lDouble&);
    void           change_sign (lDouble&);
    void           radix_point(const lDouble&);
    void           reciprocal (lDouble&);
    void           percent (lDouble&);
    void           pi (lDouble&);
    void           open_bracket (const lDouble&);
    void           close_bracket (const lDouble&);
    void           log_e (lDouble&);
    void           power_e (lDouble&);
    void           log_10 (lDouble&);
    void           power_10 (lDouble&);
    void           power (const lDouble&);
    void           square_root (lDouble&);
    void           hyperbolic (const lDouble&);
    void           arcus (const lDouble&);
    void           sine (lDouble&);
    void           cosine (lDouble&);
    void           tangent (lDouble&);
    bool           isDataEntryKey (int) const;
    bool           isOperatorKey (int) const;
    lDouble&       getValue();
    void           setDisplay (lDouble);
    void           setInfixOperator (char);
    void           clearInfixOperator();
    void           calcInfixOperator();
    void           adjustSize() override;
    const wchar_t* getButtonText (const std::size_t) const;
    void           mapKeyFunctions();

    // Event handlers
    void           onKeyPress (finalcut::FKeyEvent*) override;
    void           onClose (finalcut::FCloseEvent*) override;

    // Callback method
    void           cb_buttonClicked (Calc::button);

    // Data members
    bool              error{false};
    bool              arcus_mode{false};
    bool              hyperbolic_mode{false};
    lDouble           a{0.0L};
    lDouble           b{0.0L};
    lDouble           infinity{std::numeric_limits<lDouble>::infinity()};
    uInt              max_char{33};
    int               last_key{-1};
    char              infix_operator{'\0'};
    char              last_infix_operator{'\0'};
    finalcut::FString input{""};
    button            button_no[Calc::NUM_OF_BUTTONS]{};

    struct stack_data
    {
      lDouble term;
      char infix_operator;
    };

    std::stack<stack_data> bracket_stack{};
    std::map<Calc::button, std::shared_ptr<Button> > calculator_buttons{};
    std::map<Calc::button, keyFunction> key_map{};
};

//----------------------------------------------------------------------
Calc::Calc (FWidget* parent)
  : finalcut::FDialog{parent}
{
  // Dialog settings
  //   Avoids calling a virtual function from the constructor
  //   (CERT, OOP50-CPP)
  FDialog::setText ("Calculator");
  FDialog::setGeometry (FPoint{19, 6}, FSize{37, 18});

  mapKeyFunctions();
  clearInfixOperator();
  std::setlocale(LC_NUMERIC, "C");

  for (button key{Sine}; key < Calc::NUM_OF_BUTTONS; key = button(key + 1))
  {
    auto btn = std::make_shared<Button>(this);
    button_no[key] = key;

    if ( key == Equals )
      btn->setGeometry(FPoint{30, 15}, FSize{5, 3});
    else
    {
      const std::size_t n = ( key <= Three ) ? 0 : 1;
      const int x = int(key + n) % 5 * 7 + 2;
      const int y = int(key + n) / 5 * 2 + 3;
      btn->setGeometry(FPoint{x, y}, FSize{5, 1});
    }

    btn->setFlat();
    btn->setNoUnderline();
    btn->setText(getButtonText(key));
    btn->setDoubleFlatLine(fc::top);
    btn->setDoubleFlatLine(fc::bottom);

    if ( finalcut::FTerm::isNewFont() )
      btn->unsetClickAnimation();

    btn->addCallback
    (
      "clicked",
      this, &Calc::cb_buttonClicked,
      button_no[key]
    );

    calculator_buttons[button(key)] = btn;
  }

  calculator_buttons[On]->addAccelerator(fc::Fkey_dc);  // Del key
  calculator_buttons[On]->setFocus();
  calculator_buttons[Pi]->addAccelerator('p');
  calculator_buttons[Power]->addAccelerator('^');
  calculator_buttons[Divide]->addAccelerator('/');
  calculator_buttons[Powers_of_ten]->addAccelerator('d');
  calculator_buttons[Multiply]->addAccelerator('*');
  calculator_buttons[Decimal_point]->addAccelerator(',');
  calculator_buttons[Change_sign]->addAccelerator('#');
  calculator_buttons[Equals]->addAccelerator(fc::Fkey_return);
  calculator_buttons[Equals]->addAccelerator(fc::Fkey_enter);
}

//----------------------------------------------------------------------
Calc::~Calc()
{ }

//----------------------------------------------------------------------
void Calc::onKeyPress (finalcut::FKeyEvent* ev)
{
  const std::size_t len = input.getLength();
  const FKey key = ev->key();

  switch ( key )
  {
    case fc::Fkey_erase:
    case fc::Fkey_backspace:
      if ( len > 0 )
      {
        lDouble& x = getValue();

        if ( len == 1 )
        {
          input = "";
          x = 0.0L;
        }
        else
        {
          input = input.left(input.getLength() - 1);
          x = std::strtold(input.c_str(), nullptr);
        }

        drawDispay();
        updateTerminal();
      }

      ev->accept();
      break;

    case fc::Fkey_escape:
    case fc::Fkey_escape_mintty:
      sendOnButtonAccelerator();
      ev->accept();
      break;

    case 'q':
      close();
      ev->accept();
      break;

    default:
      finalcut::FDialog::onKeyPress(ev);
      break;
  }
}

//----------------------------------------------------------------------
void Calc::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void Calc::cb_buttonClicked (Calc::button key)
{
  lDouble& x = getValue();

  // Call the key function
  key_map[key](x);

  if ( ! input.isEmpty() )
  {
    if ( isDataEntryKey(key) )
      x = lDouble(input.toDouble());
    else
    {
      // Remove trailing zeros
      while ( ! input.includes(L'e')
           && input.includes(L'.')
           && input.back() == L'0' )
        input = input.left(input.getLength() - 1);
    }
  }

  drawDispay();
  updateTerminal();

  if ( infix_operator && ! isDataEntryKey(key) )
    input = "";

  last_key = key;
}

//----------------------------------------------------------------------
void Calc::drawDispay()
{
  finalcut::FString display{input};

  if ( display.isNull() || display.isEmpty()  )
    display = L'0';

  if ( display.right(3) == L"-0." )
    display = L'0';

  if ( display.back() == L'.' && display.getLength() > 1 )
    display = display.left(display.getLength() - 1);

  if ( ! display.isEmpty() && display.getLength() < max_char )
    display.insert(finalcut::FString{max_char - display.getLength(), L' '}, 0);

  if ( display.getLength() > max_char )
    display = display.left(max_char);

  if ( infix_operator )
    display[1] = infix_operator;

  if ( error )
    display = " Error                          ";

  if ( finalcut::FTerm::isMonochron() )
    setReverse(false);

  const auto& wc = getColorTheme();
  print() << FColorPair{fc::Black, fc::LightGray}
          << FPoint{3, 3} << display << ' '
          << FColorPair{wc->dialog_fg, wc->dialog_bg};

  if ( finalcut::FTerm::isMonochron() )
    setReverse(true);

  if ( finalcut::FTerm::isNewFont() )
  {
    const wchar_t bottom_line     {fc::NF_border_line_bottom};
    const wchar_t top_bottom_line {fc::NF_border_line_up_and_down};
    const wchar_t top_line        {fc::NF_border_line_upper};
    const wchar_t right_line      {fc::NF_rev_border_line_right};
    const wchar_t left_line       {fc::NF_border_line_left};
    print() << FPoint{3, 2} << finalcut::FString{33, bottom_line};
    print() << FPoint{2, 3} << right_line;
    print() << FPoint{36, 3} << left_line;
    print() << FPoint{3, 4};
    const finalcut::FString top_bottom_line_5 {5, top_bottom_line};
    const finalcut::FString top_line_2 {2, top_line};
    print ( top_bottom_line_5 + top_line_2
          + top_bottom_line_5 + top_line_2
          + top_bottom_line_5 + top_line_2
          + top_bottom_line_5 + top_line_2
          + top_bottom_line_5 );
  }
  else
  {
    const wchar_t vertical_and_right {fc::BoxDrawingsVerticalAndRight};
    const wchar_t horizontal         {fc::BoxDrawingsHorizontal};
    const wchar_t vertical_and_left  {fc::BoxDrawingsVerticalAndLeft};
    finalcut::FString separator ( finalcut::FString{vertical_and_right}
                                + finalcut::FString{35, horizontal}
                                + finalcut::FString{vertical_and_left} );
    print() << FPoint{1, 4} << separator;
  }
}

//----------------------------------------------------------------------
inline void Calc::sendOnButtonAccelerator()
{
  finalcut::FAccelEvent a_ev(fc::Accelerator_Event, getFocusWidget());
  calculator_buttons[On]->onAccel(&a_ev);
}

//----------------------------------------------------------------------
void Calc::clear (const lDouble&)
{
  error = false;
  arcus_mode = false;
  hyperbolic_mode = false;
  calculator_buttons[Arcus]->setChecked(false);
  calculator_buttons[Hyperbolic]->setChecked(false);
  input = "";
  clearInfixOperator();
  last_infix_operator = '\0';
  a = b = 0.0L;
}

//----------------------------------------------------------------------
void Calc::zero (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '0';
  else
    input = '0';
}

//----------------------------------------------------------------------
void Calc::one (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '1';
  else
    input = '1';
}

//----------------------------------------------------------------------
void Calc::two (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '2';
  else
    input = '2';
}

//----------------------------------------------------------------------
void Calc::three (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '3';
  else
    input = '3';
}

//----------------------------------------------------------------------
void Calc::four (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '4';
  else
    input = '4';
}

//----------------------------------------------------------------------
void Calc::five (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '5';
  else
    input = '5';
}

//----------------------------------------------------------------------
void Calc::six (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '6';
  else
    input = '6';
}

//----------------------------------------------------------------------
void Calc::seven (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '7';
  else
    input = '7';
}

//----------------------------------------------------------------------
void Calc::eight (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '8';
  else
    input = '8';
}

//----------------------------------------------------------------------
void Calc::nine (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key) )
    input += '9';
  else
    input = '9';
}

//----------------------------------------------------------------------
void Calc::add (const lDouble&)
{
  if ( ! isOperatorKey(last_key) )
    calcInfixOperator();

  setDisplay(a);
  setInfixOperator('+');
}

//----------------------------------------------------------------------
void Calc::subtract (const lDouble&)
{
  if ( ! isOperatorKey(last_key) )
    calcInfixOperator();

  setDisplay(a);
  setInfixOperator('-');
}

//----------------------------------------------------------------------
void Calc::multiply (const lDouble&)
{
  if ( ! isOperatorKey(last_key) )
    calcInfixOperator();

  setDisplay(a);
  setInfixOperator('*');
}

//----------------------------------------------------------------------
void Calc::divide (const lDouble&)
{
  if ( ! isOperatorKey(last_key) )
    calcInfixOperator();

  setDisplay(a);
  setInfixOperator('/');
}

//----------------------------------------------------------------------
void Calc::equals (const lDouble&)
{
  infix_operator = last_infix_operator;
  calcInfixOperator();
  setDisplay(a);
}

//----------------------------------------------------------------------
void Calc::change_sign (lDouble& x)
{
  x *= -1.0L;
  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::radix_point (const lDouble&)
{
  if ( input.getLength() >= max_char )
    return;

  if ( isDataEntryKey(last_key)
    && ! input.isNull()
    && ! input.isEmpty()
    && ! input.includes('.') )
    input += '.';
  else
    input = "0.";
}

//----------------------------------------------------------------------
void Calc::reciprocal (lDouble& x)
{
  if ( std::fabs(x) < LDBL_EPSILON )  // x == 0
    error = true;
  else
  {
    x = 1 / x;
    setDisplay(x);
  }
}

//----------------------------------------------------------------------
void Calc::percent (lDouble& x)
{
  infix_operator = last_infix_operator;
  x /= 100.0L;
  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::pi (lDouble& x)
{
  x = PI;
  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::open_bracket (const lDouble&)
{
  const stack_data d{ a, infix_operator };
  bracket_stack.push(d);
  clearInfixOperator();
  input = "";
  a = b = 0.0L;
  setDisplay(a);
}

//----------------------------------------------------------------------
void Calc::close_bracket (const lDouble&)
{
  if ( bracket_stack.empty() )
    return;

  calcInfixOperator();
  setDisplay(a);
  const stack_data d = bracket_stack.top();
  bracket_stack.pop();
  b = d.term;
  infix_operator = d.infix_operator;
  last_infix_operator = infix_operator;
}

//----------------------------------------------------------------------
void Calc::log_e (lDouble& x)
{
  x = std::log(x);

  if ( errno == EDOM || errno == ERANGE )
    error = true;

  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::power_e (lDouble& x)
{
  x = std::exp(x);

  if ( errno == ERANGE )
    error = true;

  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::log_10 (lDouble& x)
{
  x = std::log10(x);

  if ( errno == EDOM || errno == ERANGE )
    error = true;

  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::power_10 (lDouble& x)
{
  x = std::pow(10, x);

  if ( errno == EDOM || errno == ERANGE )
    error = true;

  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::power (const lDouble& x)
{
  if ( ! isOperatorKey(last_key) )
    calcInfixOperator();

  setDisplay(x);
  setInfixOperator('^');
}

//----------------------------------------------------------------------
void Calc::square_root (lDouble& x)
{
  x = std::sqrt(x);

  if ( errno == EDOM || errno == ERANGE )
    error = true;

  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::hyperbolic (const lDouble& x)
{
  hyperbolic_mode = ! hyperbolic_mode;
  calculator_buttons[Hyperbolic]->setChecked(hyperbolic_mode);
  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::arcus (const lDouble& x)
{
  arcus_mode = ! arcus_mode;
  calculator_buttons[Arcus]->setChecked(arcus_mode);
  setDisplay(x);
}

//----------------------------------------------------------------------
void Calc::sine (lDouble& x)
{
  if ( hyperbolic_mode )
  {
    if ( arcus_mode )
    {
      x = std::log(x + std::sqrt(x * x + 1));

      if ( errno == EDOM || errno == ERANGE )
        error = true;

      if ( std::fabs(x - infinity) < LDBL_EPSILON )  // x = ∞
        error = true;
    }
    else
      x = std::sinh(x);
  }
  else
  {
    if ( arcus_mode )
      x = std::asin(x) * 180.0L / PI;
    else if ( std::fabs(std::fmod(x, 180.0L)) < LDBL_EPSILON )  // x / 180 = 0
      x = 0.0L;
    else
      x = std::sin(x * PI / 180.0L);
  }

  if ( errno == EDOM )
    error = true;

  setDisplay(x);
  arcus_mode = false;
  hyperbolic_mode = false;
  calculator_buttons[Arcus]->setChecked(false);
  calculator_buttons[Hyperbolic]->setChecked(false);
}

//----------------------------------------------------------------------
void Calc::cosine (lDouble& x)
{
  if ( hyperbolic_mode )
  {
    if ( arcus_mode )
    {
      x = std::log(x + std::sqrt(x * x - 1));

      if ( errno == EDOM || errno == ERANGE )
        error = true;

      if ( std::fabs(x - infinity) < LDBL_EPSILON )  // x = ∞
        error = true;
    }
    else
      x = std::cosh(x);
  }
  else
  {
    if ( arcus_mode )
      x = std::acos(x) * 180.0L / PI;
    else if ( std::fabs(std::fmod(x - 90.0L, 180.0L)) < LDBL_EPSILON )  // (x - 90) / 180 == 0
      x = 0.0L;
    else
      x = std::cos(x * PI / 180.0L);
  }

  if ( errno == EDOM )
    error = true;

  setDisplay(x);
  arcus_mode = false;
  hyperbolic_mode = false;
  calculator_buttons[Arcus]->setChecked(false);
  calculator_buttons[Hyperbolic]->setChecked(false);
}

//----------------------------------------------------------------------
void Calc::tangent (lDouble& x)
{
  if ( hyperbolic_mode )
  {
    if ( arcus_mode )
      if ( x < 1 )
      {
        x = 0.5L * std::log((1 + x) / (1 - x));

        if ( errno == EDOM || errno == ERANGE )
          error = true;
      }
      else
        error = true;
    else
      x = std::tanh(x);
  }
  else
  {
    if ( arcus_mode )
      x = std::atan(x) * 180.0L / PI;
    else
    {
      // Test if (x / 180) != 0 and x / 90 == 0
      if ( std::fabs(std::fmod(x, 180.0L)) > LDBL_EPSILON
        && std::fabs(std::fmod(x, 90.0L)) < LDBL_EPSILON )
        error = true;
      else if ( std::fabs(std::fmod(x, 180.0L)) < LDBL_EPSILON )  // x / 180 == 0
        x = 0.0L;
      else
        x = std::tan(x * PI / 180.0L);
    }
  }

  if ( errno == EDOM )
    error = true;

  setDisplay(x);
  arcus_mode = false;
  hyperbolic_mode = false;
  calculator_buttons[Arcus]->setChecked(false);
  calculator_buttons[Hyperbolic]->setChecked(false);
}

//----------------------------------------------------------------------
void Calc::draw()
{
  setBold();
  setColor (fc::LightBlue, fc::Cyan);
  clearArea (getVirtualDesktop(), fc::MediumShade);
  unsetBold();
  finalcut::FDialog::draw();
  drawDispay();
}

//----------------------------------------------------------------------
bool Calc::isDataEntryKey (int key) const
{
  // Test if key is in {'.', '0'..'9'}
  const int data_entry_keys[] =
  {
    Decimal_point,
    Zero,
    One,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine
  };

  const int* iter = std::find (data_entry_keys, data_entry_keys + 11, key);

  if ( iter != data_entry_keys + 11 )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
bool Calc::isOperatorKey(int key) const
{
  // Test if key is in {'*', '/', '+', '-', '^', '='}
  const int operators[] =
  {
    Multiply,
    Divide,
    Add,
    Subtract,
    Power,
    Equals
  };

  const int* iter = std::find (operators, operators + 6, key);

  if ( iter != operators + 6 )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
lDouble& Calc::getValue()
{
  if ( infix_operator )
    return b;
  else
    return a;
}

//----------------------------------------------------------------------
void Calc::setDisplay (lDouble d)
{
  input.sprintf("%32.11Lg", d);
}

//----------------------------------------------------------------------
inline void Calc::setInfixOperator(char c)
{
  infix_operator = c;
  last_infix_operator = infix_operator;
}

//----------------------------------------------------------------------
inline void Calc::clearInfixOperator()
{
  infix_operator = '\0';
}

//----------------------------------------------------------------------
void Calc::calcInfixOperator()
{
  switch ( infix_operator )
  {
    case '*':
      if ( std::fabs(a) > LDBL_EPSILON )  // a != 0.0L
      {
        // ln(a * b) = ln(a) + ln(b)
        if ( std::log(std::abs(a)) + std::log(std::abs(b)) <= std::log(LDBL_MAX) )
          a *= b;
        else
          error = true;
      }
      else
        b = 0.0L;
      break;

    case '/':
      if ( std::fabs(b) > LDBL_EPSILON )  // b != 0.0L
        a /= b;
      else
        error = true;
      break;

    case '+':
      if ( std::fabs(a) > LDBL_EPSILON )  // a != 0.0L
      {
        if ( std::log(std::abs(a)) + std::log(std::abs(1 + b / a)) <= std::log(LDBL_MAX) )
          a += b;
        else
          error = true;
      }
      else
        a = b;
      break;

    case '-':
      if ( std::fabs(b) > LDBL_EPSILON )  // b != 0.0L
      {
        if ( std::log(std::abs(a)) + std::log(std::abs(1 - b / a)) <= std::log(LDBL_MAX) )
          a -= b;
        else
          error = true;
      }
      else
        a = b * (-1.0L);
      break;

    case '^':
      a = std::pow(a, b);

      if ( errno == EDOM || errno == ERANGE )
        error = true;
      break;

    default:
      break;
  }

  clearInfixOperator();
}

//----------------------------------------------------------------------
void Calc::adjustSize()
{
  const std::size_t pw = getDesktopWidth();
  const std::size_t ph = getDesktopHeight();
  setX (1 + int(pw - getWidth()) / 2, false);
  setY (1 + int(ph - getHeight()) / 2, false);
  finalcut::FDialog::adjustSize();
}

//----------------------------------------------------------------------
const wchar_t* Calc::getButtonText (const std::size_t key) const
{
  static const wchar_t* const button_text[Calc::NUM_OF_BUTTONS] =
  {
    L"&Sin",
    L"&Cos",
    L"&Tan",
    L"1/&x",
    L"&On",
    L"L&n",
    L"&e\x02e3",
    L"&y\x02e3",
    L"Sq&r",
    L"&\xf7",
    L"&Lg",
    L"10&\x02e3",
    L"&(",
    L"&)",
    L"&\xd7",
    L"&Hyp",
    L"&7",
    L"&8",
    L"&9",
    L"&-",
    L"&Arc",
    L"&4",
    L"&5",
    L"&6",
    L"&+",
    L"&\x03c0",
    L"&1",
    L"&2",
    L"&3",
    L"&%",
    L"&0",
    L"&.",
    L"&±",
    L"&="
  };

  return button_text[key];
}

//----------------------------------------------------------------------
void Calc::mapKeyFunctions()
{
  #define B(f) std::bind((f), this, std::placeholders::_1)  // Bind macro
  key_map[Sine] = B(&Calc::sine);                   // sin
  key_map[Cosine] = B(&Calc::cosine);               // cos
  key_map[Tangent] = B(&Calc::tangent);             // tan
  key_map[Reciprocal] = B(&Calc::reciprocal);       // 1/x
  key_map[On] = B(&Calc::clear);                    // On
  key_map[Natural_logarithm] = B(&Calc::log_e);     // ln
  key_map[Powers_of_e] = B(&Calc::power_e);         // eˣ
  key_map[Power] = B(&Calc::power);                 // yˣ
  key_map[Square_root] = B(&Calc::square_root);     // sqrt
  key_map[Divide] = B(&Calc::divide);               // ÷
  key_map[Common_logarithm] = B(&Calc::log_10);     // lg
  key_map[Powers_of_ten] = B(&Calc::power_10);      // 10ˣ
  key_map[Parenthese_l] = B(&Calc::open_bracket);   // (
  key_map[Parenthese_r] = B(&Calc::close_bracket);  // )
  key_map[Multiply] = B(&Calc::multiply);           // *
  key_map[Hyperbolic] = B(&Calc::hyperbolic);       // hyp
  key_map[Seven] = B(&Calc::seven);                 // 7
  key_map[Eight] = B(&Calc::eight);                 // 8
  key_map[Nine] = B(&Calc::nine);                   // 9
  key_map[Subtract] = B(&Calc::subtract);           // -
  key_map[Arcus] = B(&Calc::arcus);                 // arc
  key_map[Four] = B(&Calc::four);                   // 4
  key_map[Five] = B(&Calc::five);                   // 5
  key_map[Six] = B(&Calc::six);                     // 6
  key_map[Add] = B(&Calc::add);                     // +
  key_map[Pi] = B(&Calc::pi);                       // π
  key_map[One] = B(&Calc::one);                     // 1
  key_map[Two] = B(&Calc::two);                     // 2
  key_map[Three] = B(&Calc::three);                 // 3
  key_map[Percent] = B(&Calc::percent);             // %
  key_map[Zero] = B(&Calc::zero);                   // 0
  key_map[Decimal_point] = B(&Calc::radix_point);   // .
  key_map[Change_sign] = B(&Calc::change_sign);     // ±
  key_map[Equals] = B(&Calc::equals);               // =
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app(argc, argv);

  // Create a calculator object
  Calc calculator(&app);

  // Set calculator object as main widget
  finalcut::FWidget::setMainWidget(&calculator);

  // Show and start the application
  calculator.show();
  return app.exec();
}
