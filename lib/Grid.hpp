#ifndef GRID_H
#define GRID_H

#include <algorithm>
#include <sstream>
using namespace std;
#include "Tile.hpp"

class Grid {
public:
  vector<Tile> grid;

  // convenience functions for iterating
  vector<Tile>::iterator begin(){return this->grid.begin();}
  vector<Tile>::iterator end(){return this->grid.end();}

  // convenience functions for access
  Tile& operator[](int idx){return this->grid[idx];}
  Tile& at(int idx){return this->grid.at(idx);}

  // sorts the vector<Tile> into a spiral-inward pattern
  vector<Tile> spiral_sort(int rowsize, vector<Tile>& vec);

  // creates Grid from input stream (tsv file of ints < 16)
  Grid(istream& in);

  // checks if there are any unmet constraints for t
  bool validate_to(vector<Tile>::iterator t);

private:
  // used in spiral_sort, tr = Top + Right edges
  void peel_tr(vector<Tile>& vec, vector<Tile>& x, int hstart, int hend, int vstart, int vend, int rowsize, int total);
  // used in spiral_sort, bl = Bottom + Left edges
  void peel_bl(vector<Tile>& vec, vector<Tile>& x, int hstart, int hend, int vstart, int vend, int rowsize, int total);
  // printer
  friend ostream& operator<<(ostream& stream, Grid const& gr);
};

#endif
