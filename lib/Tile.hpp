#ifndef TILE_H
#define TILE_H

#include <iostream>
#include <utility>
#include <map>
#include <vector>
#include <set>
using namespace std;
// Tiles are the basic unit in the problem
// each has at least one line pointing from
// the center out to an edge
// represented here with state variable
// and accessible via u-d-l-r fxns, change
// via rotate() (clockwise)
class Tile {
  static const int UP = 1;
  static const int RIGHT = 2;
  static const int DOWN = 4;
  static const int LEFT = 8;
  static const map<int,string> uc;
  int start;

  void update();
  void make_moves();

public:
  vector<int>::iterator state;
  pair<int,int> pos;
  pair<int,int> end;
  set<pair<int,int>> points_to;
  vector<int> moves;

  friend ostream& operator<<(ostream& stream, Tile const& t);

  bool operator==(const Tile other);

  Tile(int x, pair<int,int> y);
  Tile(const Tile& o);
  void set_end(pair<int,int> r);

  void rotate();
  void rotate(int& i);
  void reset();
  bool empty();
  bool valid();
};

#endif
