/***********************************************************************
* frect.cpp - Rectangle with position and size                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2022 Markus Gans                                      *
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
#include <utility>

#include "final/util/fpoint.h"
#include "final/util/frect.h"
#include "final/util/fsize.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FRect
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FRect::FRect (const FPoint& p, const FSize& s) noexcept
  : X1{p.getX()}
  , Y1{p.getY()}
  , X2{p.getX() + int(s.getWidth()) - 1}
  , Y2{p.getY() + int(s.getHeight()) - 1}
{ }

//----------------------------------------------------------------------
FRect::FRect (const FPoint& p1, const FPoint& p2) noexcept
  : X1{p1.getX()}
  , Y1{p1.getY()}
  , X2{p2.getX()}
  , Y2{p2.getY()}
{ }


// public methods of FRect
//----------------------------------------------------------------------
auto FRect::isEmpty() const -> bool
{
  return X2 == X1 - 1 && Y2 == Y1 - 1;
}

//----------------------------------------------------------------------
auto FRect::getPos() const -> FPoint
{
  return { X1, Y1 };
}

//----------------------------------------------------------------------
auto FRect::getUpperLeftPos() const -> FPoint
{
  return { X1, Y1 };
}

//----------------------------------------------------------------------
auto FRect::getUpperRightPos() const -> FPoint
{
  return { X2, Y1 };
}

//----------------------------------------------------------------------
auto FRect::getLowerLeftPos() const -> FPoint
{
  return { X1, Y2 };
}

//----------------------------------------------------------------------
auto FRect::getLowerRightPos() const -> FPoint
{
  return { X2, Y2 };
}

//----------------------------------------------------------------------
auto FRect::getSize() const -> FSize
{
  return { getWidth(), getHeight() };
}

//----------------------------------------------------------------------
void FRect::setX1 (int n) noexcept
{
  X1 = n;
}

//----------------------------------------------------------------------
void FRect::setY1 (int n) noexcept
{
  Y1 = n;
}

//----------------------------------------------------------------------
void FRect::setX2 (int n) noexcept
{
  X2 = n;
}

//----------------------------------------------------------------------
void FRect::setY2 (int n) noexcept
{
  Y2 = n;
}

//----------------------------------------------------------------------
void FRect::setX (int n) noexcept
{
  const int dX = X2 - X1;
  X1 = n;
  X2 = X1 + dX;
}

//----------------------------------------------------------------------
void FRect::setY (int n) noexcept
{
  const int dY = Y2 - Y1;
  Y1 = n;
  Y2 = Y1 + dY;
}

//----------------------------------------------------------------------
void FRect::setPos (int x, int y) noexcept
{
  const int dX = X2 - X1;
  const int dY = Y2 - Y1;
  X1 = x;
  Y1 = y;
  X2 = X1 + dX;
  Y2 = Y1 + dY;
}

//----------------------------------------------------------------------
void FRect::setPos (const FPoint& p)
{
  const int dX = X2 - X1;
  const int dY = Y2 - Y1;
  X1 = p.getX();
  Y1 = p.getY();
  X2 = X1 + dX;
  Y2 = Y1 + dY;
}

//----------------------------------------------------------------------
void FRect::setWidth (std::size_t w) noexcept
{
  X2 = X1 + int(w) - 1;
}

//----------------------------------------------------------------------
void FRect::setHeight (std::size_t h) noexcept
{
  Y2 = Y1 + int(h) - 1;
}

//----------------------------------------------------------------------
void FRect::setSize (std::size_t width, std::size_t height) noexcept
{
  X2 = X1 + int(width) - 1;
  Y2 = Y1 + int(height) - 1;
}

//----------------------------------------------------------------------
void FRect::setSize (const FSize& s)
{
  X2 = X1 + int(s.getWidth()) - 1;
  Y2 = Y1 + int(s.getHeight()) - 1;
}

//----------------------------------------------------------------------
void FRect::setRect (const FRect& r)
{
  X1 = r.X1;
  Y1 = r.Y1;
  X2 = r.X2;
  Y2 = r.Y2;
}

//----------------------------------------------------------------------
void FRect::setRect (const FPoint& p, const FSize& s)
{
  X1 = p.getX();
  Y1 = p.getY();
  X2 = p.getX() + int(s.getWidth()) - 1;
  Y2 = p.getY() + int(s.getHeight()) - 1;
}

//----------------------------------------------------------------------
void FRect::setRect (int x, int y, std::size_t width, std::size_t height) noexcept
{
  X1 = x;
  Y1 = y;
  X2 = x + int(width) - 1;
  Y2 = y + int(height) - 1;
}

//----------------------------------------------------------------------
void FRect::setCoordinates (const FPoint& p1, const FPoint& p2)
{
  setCoordinates (p1.getX(), p1.getY(), p2.getX(), p2.getY());
}

//----------------------------------------------------------------------
void FRect::setCoordinates (int x1, int y1, int x2, int y2) noexcept
{
  X1 = x1;
  Y1 = y1;
  X2 = x2;
  Y2 = y2;
}

//----------------------------------------------------------------------
void FRect::move (int dx, int dy) noexcept
{
  X1 += dx;
  Y1 += dy;
  X2 += dx;
  Y2 += dy;
}

//----------------------------------------------------------------------
void FRect::move (const FPoint& d)
{
  X1 += d.getX();
  Y1 += d.getY();
  X2 += d.getX();
  Y2 += d.getY();
}

//----------------------------------------------------------------------
void FRect::scaleBy (int dx, int dy) noexcept
{
  X2 += dx;
  Y2 += dy;
}

//----------------------------------------------------------------------
void FRect::scaleBy (const FPoint& d)
{
  X2 += d.getX();
  Y2 += d.getY();
}

//----------------------------------------------------------------------
auto FRect::contains (int x, int y) const noexcept -> bool
{
  return x >= X1 && x <= X2
      && y >= Y1 && y <= Y2;
}

//----------------------------------------------------------------------
auto FRect::contains (const FPoint& p) const -> bool
{
  return p.getX() >= X1 && p.getX() <= X2
      && p.getY() >= Y1 && p.getY() <= Y2;
}

//----------------------------------------------------------------------
auto FRect::contains (const FRect& r) const -> bool
{
  return r.X1 >= X1 && r.X2 <= X2
      && r.Y1 >= Y1 && r.Y2 <= Y2;
}

//----------------------------------------------------------------------
auto FRect::overlap (const FRect &r) const -> bool
{
  return ( std::max(X1, r.X1) <= std::min(X2, r.X2)
        && std::max(Y1, r.Y1) <= std::min(Y2, r.Y2) );
}

//----------------------------------------------------------------------
auto FRect::intersect (const FRect& r) const -> FRect
{
  // intersection: this ∩ r
  int p1_x = std::max(X1, r.X1);
  int p1_y = std::max(Y1, r.Y1);
  int p2_x = std::min(X2, r.X2);
  int p2_y = std::min(Y2, r.Y2);
  const FPoint p1{ p1_x, p1_y };
  const FPoint p2{ p2_x, p2_y };
  return { p1, p2 };
}

//----------------------------------------------------------------------
auto FRect::combined (const FRect& r) const -> FRect
{
  // Union: this ∪ r
  int p1_x = std::min(X1, r.X1);
  int p1_y = std::min(Y1, r.Y1);
  int p2_x = std::max(X2, r.X2);
  int p2_y = std::max(Y2, r.Y2);
  const FPoint p1{ p1_x, p1_y };
  const FPoint p2{ p2_x, p2_y };
  return { p1, p2 };
}


// FRect non-member operators
//----------------------------------------------------------------------
auto operator + (const FRect& r, const FSize& s) -> FRect
{
  return { r.X1
         , r.Y1
         , std::size_t(r.X2 - r.X1) + 1 + s.getWidth()
         , std::size_t(r.Y2 - r.Y1) + 1 + s.getHeight() };
}

//----------------------------------------------------------------------
auto operator - (const FRect& r, const FSize& s) -> FRect
{
  return { r.X1
         , r.Y1
         , std::size_t(r.X2 - r.X1) + 1 - s.getWidth()
         , std::size_t(r.Y2 - r.Y1) + 1 - s.getHeight() };
}

//----------------------------------------------------------------------
auto operator == (const FRect& r1, const FRect& r2) -> bool
{
  return r1.X1 == r2.X1
      && r1.Y1 == r2.Y1
      && r1.X2 == r2.X2
      && r1.Y2 == r2.Y2;
}

//----------------------------------------------------------------------
auto operator != (const FRect& r1, const FRect& r2) -> bool
{
  return r1.X1 != r2.X1
      || r1.Y1 != r2.Y1
      || r1.X2 != r2.X2
      || r1.Y2 != r2.Y2;
}

//----------------------------------------------------------------------
auto operator << (std::ostream& outstr, const FRect& r) -> std::ostream&
{
  outstr << r.X1 << " "
         << r.Y1 << " "
         << r.X2 << " "
         << r.Y2;
  return outstr;
}

//----------------------------------------------------------------------
auto operator >> (std::istream& instr, FRect& r) -> std::istream&
{
  int x1{};
  int y1{};
  int x2{};
  int y2{};
  instr >> x1;
  instr >> y1;
  instr >> x2;
  instr >> y2;
  r.setCoordinates (x1, y1, x2, y2);
  return instr;
}

}  // namespace finalcut
