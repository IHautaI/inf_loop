#include <iostream>
#include <vector>
#include <sstream>
#include <set>
#include <map>
#include <list>
#include <algorithm>
#include <future>
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
  static const int RIGHT = 2;
  static const int DOWN = 4;
  static const int LEFT = 8;
  static const map<int,string> uc;

  int start;

  // updates points_to set using current state
  void update(){
    this->points_to.clear();

    // points up, not at the top
    if((UP & this->state) != 0 && this->pos.first != 0){
      this->points_to.insert(make_pair(this->pos.first - 1, this->pos.second));
    }
    // points down, not at the bottom
    if((DOWN & this->state) != 0 && this->pos.first != this->end.first){
      this->points_to.insert(make_pair(this->pos.first + 1, this->pos.second));
    }
    // points left, not on left edge
    if((LEFT & this->state) != 0 && this->pos.second != 0){
      this->points_to.insert(make_pair(this->pos.first, this->pos.second - 1));
    }
    // points right, not on right edge
    if((RIGHT & this->state) != 0 && this->pos.second != this->end.second){
      this->points_to.insert(make_pair(this->pos.first, this->pos.second + 1));
    }
  }

public:
  int state;
  int rotations;
  pair<int,int> pos;
  pair<int,int> end;
  set<pair<int,int>> points_to;

  bool operator==(const Tile other){
    return this->pos == other.pos;
  }

  // up-right-down-left, clockwise rotation
  // returns true if it can continue rotating
  void rotate(){
    if(this->rotations < 3){
      this->state = this->state & 8 ? ((15 & (this->state << 1)) | 1) : (15 & (this->state << 1));
      this->rotations++;
      this->update();
    }
  }

  void rotate(int i){
    for(int j = 0; j < i; j++){
      this->rotate();
    }
  }

  void reverse(){
    if(this->rotations > 0){
      this->state = this->state & 1 ? ((15 & (this->state >> 1)) | 8) : (15 & (this->state >> 1));
      this->rotations--;
      this->update();
    }
  }

  Tile(int x, pair<int,int> y):start(x),pos(y){state = start; this->update(); this->rotations=0;}

  // copy cstr
  Tile(const Tile& o){
    this->start = o.start;
    this->state = o.state;
    this->rotations = o.rotations;
    this->pos = o.pos;
    this->end = o.end;
    this->update();
  }

  void reset(){
    this->state = start;
    this->rotations = 0;
    this->update();
  }

  bool empty(){
    return !(0 | this->state);
  }

  bool exhausted(){
    return this->empty()? (this->rotations == 1) : (this->rotations == 3);
  }

  bool rev_exhausted(){
    return this->rotations == 0;
  }

  friend ostream& operator<<(ostream& stream, Tile& t){
    stream << uc.at(t.state);
    return stream;
  }

  void set_end(pair<int,int> r){
    this->end = r;
  }

  bool valid(){
    int width = this->end.first;
    int length = this->end.second;
    int i = get<0>(this->pos);
    int j = get<1>(this->pos);
    return !((i == 0 && (this->state & UP) != 0) || (i == width && (this->state & DOWN) != 0) || (j == 0 && (this->state & LEFT) != 0) || (j == length && (this->state & RIGHT) != 0));
  }

};

const map<int,string> Tile::uc = {{0,"o"}, {1,"\u2575"}, {2,"\u2576"},{3,"\u2514"}, {4,"\u2577"},
            {5,"\u2502"}, {6,"\u250C"}, {7,"\u251C"}, {8,"\u2574"},
            {9,"\u2518"}, {10,"\u2500"}, {11,"\u2534"}, {12, "\u2510"},
            {13, "\u2524"}, {14,"\u252C"},{15,"\u253C"}};

// Representation of the puzzle - rectangular set of tiles w/ states
class Grid {
public:
  vector<Tile> grid;

  vector<Tile>::iterator begin(){
    return grid.begin();
  }

  vector<Tile>::iterator end(){
    return grid.end();
  }

  vector<Tile> spiral_sort(int rowsize, vector<Tile>& vec){
    vector<Tile> x;
    peel_tr(vec, x, 0, rowsize, 0, vec.size() / rowsize, rowsize, 0);
    return x;
  }

  void peel_tr(vector<Tile>& vec, vector<Tile>& x, int hstart, int hend, int vstart, int vend, int rowsize, int total){
    // go across row
    for(auto j = hstart; j < hend && total < vec.size(); j++){
      total += 1;
      x.push_back(vec[vstart*rowsize + j]);
    }

    // go down column
    for(auto i = vstart + 1; i < vend && total < vec.size(); i++){
      total += 1;
      x.push_back(vec[i*rowsize + hend - 1]);
    }

    if(total < vec.size()){
      peel_bl(vec, x, hstart, hend - 1, vstart, vend - 1, rowsize, total);
    }
  }

  void peel_bl(vector<Tile>& vec, vector<Tile>& x, int hstart, int hend, int vstart, int vend, int rowsize, int total){
    // go across row backwards
    for(auto j = hend - 1; j >= hstart && total < vec.size(); j--){
      total += 1;
      x.push_back(vec[vend*rowsize + j]);
    }

  // go up side
    for(auto i = vend - 1; i >= vstart + 1 && total < vec.size(); i--){
      total += 1;
      x.push_back(vec[i*rowsize + hstart]);
    }

    if(total < vec.size()){
      peel_tr(vec, x, hstart + 1, hend, vstart + 1, vend, rowsize, total);
    }
  }

  friend ostream& operator<<(ostream& stream, Grid& gr){
    vector<int> printer(gr.grid.size());

    for(int i = 0; i < printer.size(); i++){
      printer[i] = i;
    }

    sort(printer.begin(), printer.end(), [&](int x, int y){return gr.grid[x].pos.first < gr.grid[y].pos.first || (gr.grid[x].pos.first == gr.grid[y].pos.first && gr.grid[x].pos.second < gr.grid[y].pos.second);});

    for(auto i : printer){
      auto x = gr.grid[i];
      if(x.pos.second == 0){
        stream << "\n";
      }
      stream << x;
    }
    stream << "\n\n";

    return stream;
  }

  Grid(istream& in){
    // create grid from stdin tsv
    vector<Tile> gr;

    vector<int> check;

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
        Tile t(y, make_pair(i,j));
        gr.push_back(t);
        j++;
      }
      check.push_back(j);
      i++;
    }

    auto last = gr.back().pos;
    for(auto& x : gr){
      x.set_end(last);
    }
    // validate size, should be rectangular, and endpoint to tiles
    for(auto& x: check){
      if(x != check[0]){
        cerr << "Invalid Input, not rectangular.  Exiting" << endl;
        exit(1);
      }
    }
    this->grid = spiral_sort(gr.back().pos.second + 1, gr);
  }

  bool validate_to(vector<Tile>::iterator t){
    set<pair<int,int>> g;
    set<pair<int,int>> h;

    // up to t, get all tiles who point to t
    for(auto x = this->grid.begin(); x < t; x++){
      for(auto& y: x->points_to){
        if(y == t->pos){
          g.insert(x->pos);
        }
      }

      // get all tiles t points to that are before it
      if(find(t->points_to.begin(), t->points_to.end(), x->pos) != t->points_to.end()){
        h.insert(x->pos);
      }
    }

    return g == h;
  }
};



// first solve strategy
void solve(Grid& grid){
  // for each idx, move set (just rotation counts < 4)
  // find valid configs for Tile at idx with that move set
  // make futures with (m+1, g) and wait for them
  std::function<vector<pair<bool, vector<int>>>(int, Grid, vector<int>)> f = [&](int idx, Grid g, vector<int> moves){
    vector<pair<bool, vector<int>>> ret;
    auto launch = (idx < 14)? std::launch::async : std::launch::deferred;
    auto it = g.begin() + idx;
    if(it < g.end() - 1){
      vector<future<vector<pair<bool,vector<int>>>>> futures; // result + moves

      moves.push_back(0);
      for(auto m = 0; m < 4; m++){
        moves[idx] = m;

        if(it->valid() && g.validate_to(it)){
          futures.push_back(std::async(launch, f, idx + 1, g, moves));
        } else {
          ret.push_back(make_pair(false, moves));
        }
        it->rotate();
      }

      for(auto& x : futures){
        auto y = x.get();
        for(auto& z : y){
          if(z.first){
            ret.push_back(z);
          }
        }
      }
    } else {
      for(auto m = 0; m < 4; m++){
        moves[idx] = m;
        if(it->valid() && g.validate_to(it)){
          ret.push_back(make_pair(true, moves));
        }
        it->rotate();
      }
    }
    return ret;
  };

  auto solutions = f(0, grid, vector<int>());

  // iterate over solutions
  int i = 0;
  for(auto x : solutions){
    // if a true solution
    if(x.first){
      // reset grid
      for(auto y = grid.grid.begin(); y < grid.grid.end(); y++){
        y->reset();
      }
      // iterate through moves, applying to grid and printing steps
      cout << "\nSolution " << i << "\n-----------------\n\n";
      for(auto i = 0; i < x.second.size(); i++){
        grid.grid[i].rotate(x.second[i]);
        cout << grid << "\n\n";
      }
      i++;

      cout << "solved\n\n";
      cout << grid << "\n\n";
      cout << "--------------------\n\n";
    }
  }

  exit(0);
}


int main(){
  Grid grid = Grid(cin);

  cout << "Input:\n";
  cout << grid << "\n";

  solve(grid);

  return 0;
}
