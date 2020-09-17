/***********************************************************************
* mouse.cpp - A small mouse-controlled drawing program                 *
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

#include <final/final.h>

namespace fc = finalcut::fc;
using finalcut::FColorPair;
using finalcut::FRect;
using finalcut::FPoint;
using finalcut::FSize;


//----------------------------------------------------------------------
// class ColorChooser
//----------------------------------------------------------------------

class ColorChooser final : public finalcut::FWidget
{
  public:
    // Constructor
    explicit ColorChooser (finalcut::FWidget* = nullptr);

    // Disable copy constructor
    ColorChooser (const ColorChooser&) = delete;

    // Destructor
    ~ColorChooser() override;

    // Disable copy assignment operator (=)
    ColorChooser& operator = (const ColorChooser&) = delete;

    // Accessors
    FColor getForeground() const;
    FColor getBackground() const;

  private:
    // Mutator
    void setSize (const FSize&, bool = true) override;

    // Method
    void draw() override;
    void drawBorder() override;

    // Event handler
    void onMouseDown (finalcut::FMouseEvent*) override;

    // Data members
    FColor fg_color{fc::White};
    FColor bg_color{fc::Black};
    finalcut::FLabel headline{this};
};


//----------------------------------------------------------------------
ColorChooser::ColorChooser (finalcut::FWidget* parent)
  : FWidget{parent}
{
  FWidget::setSize (FSize{8, 12});
  setFixedSize (FSize{8, 12});
  unsetFocusable();

  // Text label
  headline.setGeometry (FPoint{1, 1}, FSize{8, 1});
  headline.setEmphasis();
  headline.setAlignment (fc::alignCenter);
  headline << "Color";
}

//----------------------------------------------------------------------
ColorChooser::~ColorChooser()
{ }

//----------------------------------------------------------------------
inline FColor ColorChooser::getForeground() const
{
  return fg_color;
}

//----------------------------------------------------------------------
inline FColor ColorChooser::getBackground() const
{
  return bg_color;
}

//----------------------------------------------------------------------
void ColorChooser::setSize (const FSize& size, bool adjust)
{
  // Avoids calling a virtual function from the constructor
  // (CERT, OOP50-CPP)
  FWidget::setSize (size, adjust);
}

//----------------------------------------------------------------------
void ColorChooser::draw()
{
  useParentWidgetColor();
  headline.setBackgroundColor(getBackgroundColor());
  setColor();
  drawBorder();

  for (FColor c{0}; c < 16; c++)
  {
    print() << FPoint{2 + (c / 8) * 3, 3 + c % 8};

    if ( c < 6 )
      setColor (fc::LightGray, c);
    else if ( c > 8 )
      setColor (fc::DarkGray, c);
    else
      setColor (fc::White, c);

    if ( c == bg_color )
    {
      print() << L' ' << wchar_t(fc::Times) << L' ';
    }
    else
      print ("   ");
  }
}

//----------------------------------------------------------------------
void ColorChooser::drawBorder()
{
  finalcut::drawBorder (this, FRect{FPoint{1, 2}, FSize{8, 10}});
}

//----------------------------------------------------------------------
void ColorChooser::onMouseDown (finalcut::FMouseEvent* ev)
{
  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

  if ( ev->getButton() == fc::MiddleButton )
    return;

  for (int c{0}; c < 16; c++)
  {
    const int xmin = 2 + (c / 8) * 3;
    const int xmax = 4 + (c / 8) * 3;
    const int y = 3 + c % 8;

    if ( mouse_x >= xmin && mouse_x <= xmax && mouse_y == y )
    {
      if ( ev->getButton() == fc::LeftButton )
        bg_color = FColor(c);
      else if ( ev->getButton() == fc::RightButton )
        fg_color = FColor(c);

      redraw();
      emitCallback("clicked");
    }
  }
}


//----------------------------------------------------------------------
// class Brushes
//----------------------------------------------------------------------

class Brushes final : public finalcut::FWidget
{
  public:
    // Constructor
    explicit Brushes (finalcut::FWidget* = nullptr);

    // Disable copy constructor
    Brushes (const Brushes&) = delete;

    // Destructor
    ~Brushes() override;

    // Disable copy assignment operator (=)
    Brushes& operator = (const Brushes&) = delete;

    // Accessor
    wchar_t getBrush() const;

    // Mutators
    void setForeground (FColor);
    void setBackground (FColor);

  private:
    // Mutator
    void setSize (const FSize&, bool = true) override;

    // Method
    void draw() override;
    void drawBorder() override;

    // Event handler
    void onMouseDown (finalcut::FMouseEvent*) override;

    // Data members
    wchar_t brush{L' '};
    FColor  fg_color{fc::White};
    FColor  bg_color{fc::Black};
    finalcut::FLabel headline{this};
};

//----------------------------------------------------------------------
Brushes::Brushes (finalcut::FWidget* parent)
  : FWidget{parent}
{
  FWidget::setSize (FSize{8, 4});
  setFixedSize (FSize{8, 4});
  unsetFocusable();

  // Text label
  headline.setGeometry(FPoint{1, 1}, FSize{8, 1});
  headline.setEmphasis();
  headline.setAlignment (fc::alignCenter);
  headline << "Brush";
}

//----------------------------------------------------------------------
Brushes::~Brushes()
{ }

//----------------------------------------------------------------------
void Brushes::setSize (const FSize& size, bool adjust)
{
  // Avoids calling a virtual function from the constructor
  // (CERT, OOP50-CPP)
  FWidget::setSize (size, adjust);
}

//----------------------------------------------------------------------
void Brushes::draw()
{
  int pos{0};
  useParentWidgetColor();
  headline.setBackgroundColor(getBackgroundColor());
  setColor();
  drawBorder();
  print() << FPoint{2, 3}
          << FColorPair{fg_color, bg_color} << "   "
          << finalcut::FString{3, fc::MediumShade};

  if ( brush != L' ' )
    pos = 3;

  setColor();
  print() << FPoint{3 + pos, 2}
          << fc::BlackDownPointingTriangle
          << FPoint{3 + pos, 4}
          << fc::BlackUpPointingTriangle;
}

//----------------------------------------------------------------------
void Brushes::drawBorder()
{
  finalcut::drawBorder (this, FRect{FPoint{1, 2}, FSize{8, 3}});
}

//----------------------------------------------------------------------
void Brushes::onMouseDown (finalcut::FMouseEvent* ev)
{
  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( mouse_x >= 2 && mouse_x <= 4 && mouse_y == 3 )
  {
    brush = L' ';
    redraw();
  }
  else if ( mouse_x >= 5 && mouse_x <= 7 && mouse_y == 3 )
  {
    brush = fc::MediumShade;
    redraw();
  }
}

//----------------------------------------------------------------------
inline wchar_t Brushes::getBrush() const
{
  return brush;
}

//----------------------------------------------------------------------
inline void Brushes::setForeground (FColor color)
{
  fg_color = color;
}

//----------------------------------------------------------------------
inline void Brushes::setBackground (FColor color)
{
  bg_color = color;
}


//----------------------------------------------------------------------
// class MouseDraw
//----------------------------------------------------------------------

class MouseDraw final : public finalcut::FDialog
{
  public:
    // Using-declaration
    using FWidget::setGeometry;

    // Constructor
    explicit MouseDraw (finalcut::FWidget* = nullptr);

    // Disable copy constructor
    MouseDraw (const MouseDraw&) = delete;

    // Destructor
    ~MouseDraw() override;

    // Disable copy assignment operator (=)
    MouseDraw& operator = (const MouseDraw&) = delete;

    // Methods
    void setGeometry (const FPoint&, const FSize&, bool = true) override;

    // Event handlers
    void onKeyPress (finalcut::FKeyEvent*) override;
    void onClose (finalcut::FCloseEvent*) override;

  private:
    // Methods
    void draw() override;
    void drawBrush (int, int, bool = false);
    void drawCanvas();
    void createCanvas();
    void adjustSize() override;

    // Event handler
    void onMouseDown (finalcut::FMouseEvent*) override;
    void onMouseMove (finalcut::FMouseEvent*) override;

    // Callback methods
    void cb_colorChanged();

    // Data members
    FTermArea*   canvas{nullptr};
    ColorChooser c_chooser{this};
    Brushes      brush{this};
};


//----------------------------------------------------------------------
MouseDraw::MouseDraw (finalcut::FWidget* parent)
  : finalcut::FDialog{parent}
{
  FDialog::setText ("Drawing with the mouse");
  c_chooser.setPos (FPoint{1, 1});
  c_chooser.addCallback
  (
    "clicked",
    this, &MouseDraw::cb_colorChanged
  );

  brush.setPos (FPoint{1, 12});
}

//----------------------------------------------------------------------
MouseDraw::~MouseDraw()
{ }

//----------------------------------------------------------------------
void MouseDraw::setGeometry ( const FPoint& p, const FSize& s, bool adjust)
{
  finalcut::FDialog::setGeometry (p, s, adjust);
  const std::size_t w = s.getWidth();
  const std::size_t h = s.getHeight();
  const finalcut::FRect scroll_geometry (FPoint{0, 0}, FSize{w - 11, h - 3});

  if ( ! canvas )
    return;

  const FSize no_shadow{0, 0};
  const int old_w = canvas->width;
  const int old_h = canvas->height;
  resizeArea (scroll_geometry, no_shadow, canvas);

  if ( old_w != canvas->width || old_h != canvas->height )
  {
    setColor(getForegroundColor(), getBackgroundColor());
    clearArea (canvas, ' ');
  }
}

//----------------------------------------------------------------------
void MouseDraw::onKeyPress (finalcut::FKeyEvent* ev)
{
  if ( ! ev )
    return;

  if ( ev->key() == 'q' )
  {
    close();
    ev->accept();
  }
  else
    finalcut::FDialog::onKeyPress(ev);
}

//----------------------------------------------------------------------
void MouseDraw::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void MouseDraw::draw()
{
  const int y_max = int(getHeight());
  finalcut::FDialog::draw();
  setColor();

  if ( finalcut::FTerm::isNewFont() )
  {
    for (int y{2}; y < y_max; y++)
    {
      print() << FPoint{10, y}
              << fc::NF_rev_border_line_right;
    }

    print() << FPoint{10, y_max}
            << fc::NF_rev_border_corner_lower_right;
  }
  else
  {
    print() << FPoint{10, 2}
            << fc::BoxDrawingsDownAndHorizontal;

    for (int y{3}; y < y_max; y++)
    {
      print() << FPoint{10, y} << fc::BoxDrawingsVertical;
    }

    print() << FPoint{10, y_max}
            << fc::BoxDrawingsUpAndHorizontal;
  }

  drawCanvas();
}

//----------------------------------------------------------------------
void MouseDraw::drawBrush (int x, int y, bool swap_color)
{
  const int Cols = int(getWidth());
  const int Lines = int(getHeight());

  if ( x > 10 && x < Cols && y > 2 && y < Lines )
  {
    if ( swap_color )
      setColor (c_chooser.getBackground(), c_chooser.getForeground());
    else
      setColor (c_chooser.getForeground(), c_chooser.getBackground());

    // set canvas print cursor position
    canvas->cursor_x = x - canvas->offset_left - 10;
    canvas->cursor_y = y - canvas->offset_top - 2;
    // print on canvas
    print (canvas, brush.getBrush());
    // copy canvas to the dialog
    drawCanvas();
  }
}

//----------------------------------------------------------------------
void MouseDraw::drawCanvas()
{
  if ( ! hasPrintArea() )
    finalcut::FVTerm::getPrintArea();

  // Create canvas after initializing the desktop and color theme
  if ( ! canvas )
    createCanvas();

  if ( ! (hasPrintArea() && canvas) )
    return;

  auto printarea = getCurrentPrintArea();
  const int ax = 9 + getTermX() - printarea->offset_left;
  const int ay = 1 + getTermY() - printarea->offset_top;
  const int y_end = canvas->height;
  const int x_end = canvas->width;
  const int w_line_len = printarea->width + printarea->right_shadow;

  for (int y{0}; y < y_end; y++)  // line loop
  {
    const finalcut::FChar* canvaschar{};  // canvas character
    finalcut::FChar* winchar{};     // window character
    canvaschar = &canvas->data[y * x_end];
    winchar = &printarea->data[(ay + y) * w_line_len + ax];
    std::memcpy ( winchar
                , canvaschar
                , sizeof(finalcut::FChar) * unsigned(x_end) );

    if ( int(printarea->changes[ay + y].xmin) > ax )
      printarea->changes[ay + y].xmin = uInt(ax);

    if ( int(printarea->changes[ay + y].xmax) < ax + x_end - 1 )
      printarea->changes[ay + y].xmax = uInt(ax + x_end - 1);
  }

  printarea->has_changes = true;
}

//----------------------------------------------------------------------
void MouseDraw::createCanvas()
{
  FSize no_shadow{0, 0};
  finalcut::FRect scroll_geometry{0, 0, 1, 1};
  createArea (scroll_geometry, no_shadow, canvas);
  adjustSize();
}

//----------------------------------------------------------------------
void MouseDraw::adjustSize()
{
  const std::size_t w{60};
  const std::size_t h{18};
  const int x = 1 + int((getParentWidget()->getWidth() - w) / 2);
  const int y = 1 + int((getParentWidget()->getHeight() - h) / 2);
  setGeometry (FPoint{x, y}, FSize{w, h}, false);
  finalcut::FDialog::adjustSize();
}

//----------------------------------------------------------------------
void MouseDraw::onMouseDown (finalcut::FMouseEvent* ev)
{
  finalcut::FDialog::onMouseDown(ev);

  if ( ev->getButton() != fc::LeftButton
    && ev->getButton() != fc::RightButton )
    return;

  drawBrush ( ev->getX()
            , ev->getY()
            , ev->getButton() == fc::RightButton );
}

//----------------------------------------------------------------------
void MouseDraw::onMouseMove (finalcut::FMouseEvent* ev)
{
  FDialog::onMouseMove(ev);

  if ( ev->getButton() != fc::LeftButton
    && ev->getButton() != fc::RightButton )
    return;

  drawBrush ( ev->getX()
            , ev->getY()
            , ev->getButton() == fc::RightButton);
}

//----------------------------------------------------------------------
void MouseDraw::cb_colorChanged()
{
  brush.setForeground (c_chooser.getForeground());
  brush.setBackground (c_chooser.getBackground());
  brush.redraw();
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app{argc, argv};

  // Create a simple dialog box
  MouseDraw mouse_draw{&app};
  mouse_draw.setGeometry (FPoint{12, 4}, FSize{60, 18});

  // Set dialog object mouse_draw as main widget
  finalcut::FWidget::setMainWidget(&mouse_draw);

  // Show and start the application
  mouse_draw.show();
  return app.exec();
}
