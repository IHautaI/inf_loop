#include <iostream>
#include <vector>
#include <sstream>
#include <tuple>
#include <set>
#include <map>
#include <list>
using namespace std;


void end(){
  cout << "No Solution\n";
  exit(0);
}


// convenience fxn for swapping the 2tuples in a 2tuple(2tuples(ints))
tuple<tuple<int,int>,tuple<int,int>> swap_tuple(const tuple<tuple<int,int>,tuple<int,int>>& tup){
  auto x = get<0>(tup);
  auto y = get<1>(tup);
  return make_tuple(y,x);
}


// Tiles are the basic unit in the problem
// each has at least one line pointing from
// the center out to an edge
// represented here with state variable
// and accessible via u-d-l-r fxns, change
// via rotate() (clockwise)
class Tile {
  static const int UP = 1;
  static const int DOWN = 4;
  static const int LEFT = 8;
  static const int RIGHT = 2;
  int start;
  int state;

public:
  int rotations;
  // up-right-down-left, clockwise rotation
  void rotate(){
    int mask = this->state & 1;
    this->state = (this->state << 1) | mask;
    this->rotations++;
  }

  Tile(int x):start(x){this->state = start;}

  void reset(){
    this->state = start;
    this->rotations = 0;
  }

  bool up(){
    return UP & this->state;
  }

  bool down(){
    return DOWN & this->state;
  }

  bool left(){
    return LEFT & this->state;
  }

  bool right(){
    return RIGHT & this->state;
  }

  bool empty(){
    return 0 & this->state;
  }

  friend ostream& operator<<(ostream& stream, const Tile& t){
    stream << t.state;
    return stream;
  }

  bool valid(tuple<int,int> pos, int width, int length){
    int i = get<0>(pos);
    int j = get<1>(pos);

    return !((i == 0 && this->up()) || (i == width && this->down()) || (j==0 && this->left()) || (j == length && this->right()));
  }

};


// Representation of the puzzle - rectangular set of tiles w/ states
class Grid {
public:
  map<tuple<int,int>,Tile> grid;
  int length;
  int width;

  friend ostream& operator<<(ostream& stream, Grid& gr){
    for(auto i = 0; i<gr.width; i++){
      for(auto j = 0; j < gr.length; i++){
        stream << gr.grid[make_tuple(i,j)] << " ";
      }
      stream << "\n";
    }
    return stream;
  }

  Grid(istream& in){
    // create grid from stdin tsv
    map<tuple<int,int>,Tile> gr;
    stringstream ss;
    string line;

    int i = 0;
    // read in lines one-by-one into line
    while(getline(in, line) && !line.empty()){
      // setup stream from line
      ss.clear();
      ss.str("");
      ss << line;

      // extract tiles from stringstream, put in map
      int y;
      int j = 0;
      while(ss >> y){
        gr[make_tuple(i,j)] = Tile(y);
        j++;
      }
      i++;
    }

    // validate size (needs to be rectangular)
    int target = gr.size() % i;
    map<int, int> check;
    for(auto y = 0; y < i; y++){
      check[y] = 0;
    }

    for(auto x : gr){
      check[get<0>(x.first)] += get<1>(x.first);
    }

    for(auto x : check){
      if(x.second != target){
        cout << "Input not rectangular!";
        exit(1);
      }
    }
    // set values
    this->grid = gr;
    this->width = i;
    this->length = target;
  }

  // convenience fxn for rotating tiles
  void rotate(tuple<int,int> pos){
    this->grid[pos].rotate();
  }

};

// Class for manipulating an edge list for a Grid
class Edge {
public:
  set<tuple<tuple<int,int>,tuple<int,int>>> edges;

  void add_values(tuple<int,int> pos, Tile& tile, int width, int length){
    int i = get<0>(pos);
    int j = get<1>(pos);

    if(tile.up() && i > 0){
      this->edges.insert(pos, make_tuple(i-1, j));
    }
    if(tile.down() && i < width - 1){
      this->edges.insert(pos, make_tuple(i+1, j));
    }
    if(tile.left() && j > 0){
      this->edges.insert(pos, make_tuple(i, j-1));
    }
    if(tile.right() && j < length - 1){
      this->edges.insert(pos, make_tuple(i, j+1));
    }
  }

  friend ostream& operator<<(ostream& stream, const Edge& edge){
    for(auto x : edge.edges){
      stream << "((" << get<0>(get<0>(x)) << "," << get<1>(get<0>(x)) << "),(" << get<0>(get<1>(x)) << "," << get<1>(get<1>(x)) << ")) ";
    }
    return stream;
  }

  // fill edge list for given grid
  void fill_edges(Grid& grid){
    this->edges.clear();

    for(auto x : grid.grid){
        this->add_values(x.first, x.second, grid.width, grid.length);
    }
  }

  // fill + check validity
  bool complete(Grid& grid){
    this->fill_edges(grid);
    return this->valid();
  }

private:
  // checks if the current edge list is a valid solution
  bool valid(){
    for(const auto& x: this->edges){
      set<tuple<tuple<int,int>,tuple<int,int>>>::const_iterator got = this->edges.find(swap_tuple(x));
      if(got == this->edges.end()){
        return false;
      }
    }
    return true;
  }
};


// first solve strategy
void solve(Grid& grid){
  list<tuple<tuple<int,int>,tuple<int,int>>> history;

  Edge edges;

  auto it = grid.grid.begin();

  auto f = [&](){
    while(it != grid.grid.end()){
      auto tile = it->second;
      auto pos = it->first;

      while(!tile.valid(pos, grid.width, grid.length) && tile.rotations < 4){
        tile.rotate();
      }

      // if no valid configuration
      if(tile.rotations){
        // reset tile
        tile.reset();

        // fill edges
        edges.fill_edges(grid);

        set<tuple<tuple<int,int>,tuple<int,int>>> tmp;
        // search edges for all that point to tile
        for(auto x : edges.edges){
          if(get<1>(x) == pos){
            tmp.insert(x);
          }
        }

        // if none point to it, backtrack
        if(tmp.size() != 0){
          auto y = history.back();

          // find last tile that pointed to this one
          while(tmp.find(y) == tmp.end() && history.size() != 0){
            // reset it and drop from history
            grid.grid[get<0>(y)].reset();
            history.pop_back();

            // grab next
            y = history.back();
          }

          // drop all other entries for y
          {
            auto x = history.back();
            while(get<0>(x) == get<0>(y)){
              history.pop_back();
              x = history.back();
            }
          }
          // if you can't backtrack, that's the end
          if(history.size() == 0){
            end();
          }

          // get it's position
          pos = get<0>(y);
          // rotate once
          grid.grid[pos].rotate();
          // set iterator
          it = grid.grid.find(pos);

          // and start again
          return false;
        } else {
          // locked tile, but none point to it == the end
          end();
        }
      }
      it++;
    }
    return true;
  };
  while(!f());
}


int main(){
  Grid grid = Grid(cin);
  Edge edge;

  // while not solved, solve it...
  while(!edge.complete(grid)){
    // for debugging...
    cout << edge;
    solve(grid);
  }

  cout << "\n";
  cout << edge;
  // make room
  cout << "\n\n";
  // print solved grid
  cout << grid;

  return 0;
}
