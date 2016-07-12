#include <iostream>
#include <vector>
#include <sstream>
#include <tuple>
#include <set>
using namespace std;

// convenience fxn for making tuple of 2tuples of ints
tuple<tuple<int,int>,tuple<int,int>> quad_tuple(int i, int j, int k, int l){
  return make_tuple(make_tuple(i,j), make_tuple(k,l));
}


// convenience fxn for swapping the 2tuples in said tuple
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
  int state;

public:
  // up-right-down-left, clockwise rotation
  void rotate(){
    int mask = this->state & 1;
    this->state = (this->state << 1) | mask;
  }

  Tile(int x):state(x){}

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

  friend ostream& operator<<(ostream& stream, const Tile& t){
    stream << t.state;
    return stream;
  }

};


// Representation of the puzzle - rectangular set of tiles w/ states
class Grid {
public:
  vector<vector<Tile>> grid;
  int length;
  int width;

  friend ostream& operator<<(ostream& stream, Grid& gr){
    for(auto x : gr.grid){
      for(auto y : x){
        stream << y << " ";
      }
      stream << "\n";
    }
    return stream;
  }

  Grid(istream& in){
    // create grid from stdin tsv
    vector<vector<Tile>> gr;
    stringstream ss;
    string line;

    // read in lines one-by-one into line
    while(getline(in, line) && !line.empty()){
      // setup stream from line
      ss.clear();
      ss.str("");
      ss << line;

      vector<Tile> vec;
      // extract tiles from stringstream, insert into vector
      int y;
      while(ss >> y){
        vec.push_back(Tile(y));
      }
      // insern vector into vector
      gr.push_back(vec);
    }

    // validate vector lengths (needs to be rectangular)
    auto k = gr[0].size();
    for(auto i = 1; i < gr.size(); i++){
      if(gr[i].size() != k){
        cout << "Dimension Mismatch!";
        exit(1);
      }
    }

    this->grid = gr;
    this->width = gr.size();
    this->length = k;
  }

  // convenience fxn for rotating tiles
  void rotate(int i,int j){
    this->grid[i][j].rotate();
  }

};

// Class for manipulating an edge list for a Grid
class Edge {
public:
  set<tuple<tuple<int,int>,tuple<int,int>>> edges;

  void add_values(int i, int j, Tile& tile, int width, int length){
    if(tile.up() && i > 0){
      this->edges.insert(quad_tuple(i,j,i-1,j));
    }
    if(tile.down() && i < width - 1){
      this->edges.insert(quad_tuple(i,j,i+1,j));
    }
    if(tile.left() && j > 0){
      this->edges.insert(quad_tuple(i,j,i,j-1));
    }
    if(tile.right() && j < length - 1){
      this->edges.insert(quad_tuple(i,j,i,j+1));
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

    for(int i = 0; i < grid.width; i++){
      for(int j = 0; j < grid.length; j++){
        this->add_values(i, j, grid.grid[i][j], grid.width, grid.length);
      }
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


// solve strategy
void solve(Grid& grid){
  return;
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

  // make room
  cout << "\n\n";
  // print solved grid
  cout << grid;

  return 0;
}
