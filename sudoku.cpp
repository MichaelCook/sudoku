/*
  Sudoku puzzle solver.

  Example:

  $ g++ -std=c++11 -O3 -DNDEBUG sudoku.cpp
  $
  $ cat sudoku1.txt
  __3 86_ ___
  _2_ ___ 6_3
  1__ ___ _2_

  ___ 3__ _4_
  __7 __1 ___
  __5 __4 3__

  ___ _1_ _74
  27_ __3 __1
  9__ 2__ ___
  $
  $ ./a.out <sudoku1.txt
  753 862 419
  429 175 683
  186 439 725
  692 358 147
  347 691 852
  815 724 396
  538 916 274
  274 583 961
  961 247 538

  $

  ---

  opt="-O0 -g      "; g++ -Wall -Werror -std=c++11 $opt sudoku.cpp && ./a.out <sudoku1.txt
  opt="-O3 -DNDEBUG"; g++ -Wall -Werror -std=c++11 $opt sudoku.cpp && ./a.out <sudoku1.txt

*/

#include <iostream>
#include <stdexcept>
#include <cctype>
#include <cassert>

using std::cout;
using std::endl;

struct Cell {
  int assigned_; // the digit 1-9 assigned to this cell or 0 if unassigned
  unsigned assignable_; // bits 1-9 indicating which digit could be assigned
  bool assignable() const {
    return assignable_;
  }
  bool assignable(int dig) const {
    assert(dig >= 1 && dig <= 9);
    return assignable_ & (1 << dig);
  }
  void cant_assign(int dig) {
    assert(dig >= 1 && dig <= 9);
    assignable_ &= ~(1 << dig);
  }

  Cell() : assigned_(0),
           assignable_((1 << 9) |
                       (1 << 8) |
                       (1 << 7) |
                       (1 << 6) |
                       (1 << 5) |
                       (1 << 4) |
                       (1 << 3) |
                       (1 << 2) |
                       (1 << 1))
  {}
};

struct Board {
  static constexpr int num_cells = 9 * 9;
  Cell cells[num_cells];
  void find(int pos) const;
  Cell& cell(int pos) {
    assert(pos >= 0 && pos < num_cells);
    return cells[pos];
  }
  Cell const& cell(int pos) const {
    assert(pos >= 0 && pos < num_cells);
    return cells[pos];
  }
  Cell& cell(int row, int col) {
    assert(row >= 0 && row < 9);
    assert(col >= 0 && col < 9);
    return cells[row * 9 + col];
  }
  Cell const& cell(int row, int col) const {
    assert(row >= 0 && row < 9);
    assert(col >= 0 && col < 9);
    return cells[row * 9 + col];
  }
  void print() const;
  void assign(int pos, int dig);
};

void Board::find(int pos) const
{
  for (;;) {
    if (pos == num_cells) {
      print();
      return;
    }
    if (cell(pos).assigned_ == 0)
      break;
    ++pos;
  }
  auto& x = cell(pos);
  if (!x.assignable())
    return;
  for (int dig = 1; dig <= 9; ++dig) {
    if (x.assignable(dig)) {
      Board next(*this);
      next.assign(pos, dig);
      next.find(pos);
    }
  }
}

void Board::print() const
{
  for (int r = 0; r < 9; ++r) {
    for (int c = 0; c < 9; ++c) {
      auto& x = cell(r, c);
      if (x.assigned_)
        cout << x.assigned_;
      else
        cout << '_';
      if (c == 2 || c == 5)
        cout << ' ';
    }
    cout << '\n';
  }
  cout << '\n';
}

void Board::assign(int pos, int dig)
{
  assert(pos >= 0 && pos < num_cells);
  assert(dig >= 1 && dig <= 9);
  int row = pos / 9;
  int col = pos % 9;
  cell(pos).assigned_ = dig;
  for (int r = 0; r < 9; ++r)
    cell(r, col).cant_assign(dig);
  for (int c = 0; c < 9; ++c)
    cell(row, c).cant_assign(dig);
  int rb = row - row % 3;
  int cb = col - col % 3;
  for (int r = 0; r < 3; ++r)
    for (int c = 0; c < 3; ++c)
      cell(rb + r, cb + c).cant_assign(dig);
}

int main()
{
  Board b;
  int c;
  int pos = 0;
  while ((c = getchar()) != EOF) {
    if (c == '_')
      ++pos;
    else if (isdigit(c)) {
      if (pos < Board::num_cells)
        b.assign(pos, c - '0');
      ++pos;
    }
  }
  if (pos != Board::num_cells)
    throw std::runtime_error("Invalid board: " + std::to_string(pos));
  b.find(0);
}
