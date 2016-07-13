#include <iostream>
#include <vector>
#include <sstream>
#include <set>
#include <map>
#include <list>
using namespace std;


void end(){
  cout << "No Solution\n";
  exit(0);
}


// convenience fxn for swapping the pairs in a pair(pairs(ints))
pair<pair<int,int>,pair<int,int>> swap_pair(const pair<pair<int,int>,pair<int,int>>& tup){
  auto x = get<0>(tup);
  auto y = get<1>(tup);
  return make_pair(y,x);
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

  Tile(int x):start(x){state = start;}

  // copy cstr
  Tile(const Tile& o){
    start = o.start;
    state = o.start;
    rotations = 0;
  }

  // // move cstr
  // Tile(Tile&& o): start(std::move(o.start)){
  //   this->state = o.start;
  //   this->rotations = 0;
  // }

  // // copy assignment
  // Tile& operator=(Tile other){
  //   if(&other == this){
  //     return *this;
  //   }
  //   swap(start, other.start);
  //   swap(state, other.state);
  //   return *this;
  // }

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

  bool valid(pair<int,int> pos, int width, int length){
    int i = get<0>(pos);
    int j = get<1>(pos);

    return !((i == 0 && this->up()) || (i == width && this->down()) || (j==0 && this->left()) || (j == length && this->right()));
  }

};


// Representation of the puzzle - rectangular set of tiles w/ states
class Grid {
public:
  map<pair<int,int>,Tile> grid;
  int length;
  int width;

  friend ostream& operator<<(ostream& stream, Grid& gr){
    for(auto i = 0; i<gr.width; i++){
      for(auto j = 0; j < gr.length; i++){
        stream << gr.grid.find(make_pair(i,j))->second << " ";
      }
      stream << "\n";
    }
    return stream;
  }

  Grid(istream& in){
    // create grid from stdin tsv
    map<pair<int,int>,Tile> gr;
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
        Tile t(y);
        gr.insert(make_pair(make_pair(i,j),t));
        // gr[make_pair(i,j)] = t;
        j++;
      }
      i++;
    }

    // validate size (needs to be rectangular)
    int target = gr.size() % i;
    map<int,int> check;

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
  void rotate(pair<int,int> pos){
    this->grid.find(pos)->second.rotate();
  }

};

// Class for manipulating an edge list for a Grid
class Edge {
public:
  set<pair<pair<int,int>,pair<int,int>>> edges;

  void add_values(pair<int,int> pos, Tile& tile, int width, int length){
    int i = get<0>(pos);
    int j = get<1>(pos);

    if(tile.up() && i > 0){
      this->edges.insert(make_pair(pos, make_pair(i-1, j)));
    }
    if(tile.down() && i < width - 1){
      this->edges.insert(make_pair(pos, make_pair(i+1, j)));
    }
    if(tile.left() && j > 0){
      this->edges.insert(make_pair(pos, make_pair(i, j-1)));
    }
    if(tile.right() && j < length - 1){
      this->edges.insert(make_pair(pos, make_pair(i, j+1)));
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
      set<pair<pair<int,int>,pair<int,int>>>::const_iterator got = this->edges.find(swap_pair(x));
      if(got == this->edges.end()){
        return false;
      }
    }
    return true;
  }
};


// first solve strategy
void solve(Grid& grid){
  list<pair<pair<int,int>,pair<int,int>>> history;

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
      if(tile.rotations == 4){
        // reset tile
        tile.reset();

        // fill edges
        edges.fill_edges(grid);

        set<pair<pair<int,int>,pair<int,int>>> tmp;
        // search edges for all that point to tile
        for(auto x : edges.edges){
          if(get<1>(x) == pos){
            tmp.insert(x);
          }
        }

        // if something points to it, backtrack to them
        if(!tmp.empty()){
          auto y = history.back();

          // find last tile that pointed to this one
          while(tmp.find(y) == tmp.end() && !history.empty()){
            // reset it and drop from history
            grid.grid.find(get<0>(y))->second.reset();
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
          if(history.empty()){
            end();
          }

          // get it's position
          pos = get<0>(y);
          // rotate once
          grid.grid.find(pos)->second.rotate();
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
    //solve(grid);
  }

  cout << "\n";
  cout << edge;
  // make room
  cout << "\n\n";
  // print solved grid
  cout << grid;

  return 0;
}
