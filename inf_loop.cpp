#include <iostream>
#include <vector>
#include <sstream>
#include <set>
#include <map>
#include <list>
#include <algorithm>
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
    start = o.start;
    state = o.start;
    rotations = 0;
    pos = o.pos;
    end = o.end;
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

  friend ostream& operator<<(ostream& stream, const Tile& t){
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

  friend ostream& operator<<(ostream& stream, Grid& gr){
    for(auto& x : gr.grid){
      if(x.pos.second == 0){
        stream << "\n";
      }
      stream << x;
    }
    stream << "\n";

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
    this->grid = gr;
  }

  bool validate_to(vector<Tile>::iterator t){
    set<pair<int,int>> g;
    set<pair<int,int>> h;
    // cout << "t state "<< t->state;

    // not needed, just for logging process
    // for(auto x = t->points_to.begin(); x != t->points_to.end(); x++){
    //   cout << ", points_to " << x->first << "," << x->second;
    // }

    // up to t, get all tiles who point to t
    for(auto x = this->grid.begin(); x < t; x++){
      // cout << "\n";
      // cout << x->pos.first << "," << x->pos.second << " state " << x->state <<", points_to ";
      for(auto& y: x->points_to){
        // cout << y.first << "," << y.second << " ";
        if(y == t->pos){
          g.insert(x->pos);
        }
      }

      // get all tiles t points to that are before it
      if(find(t->points_to.begin(), t->points_to.end(), x->pos) != t->points_to.end()){
        h.insert(x->pos);
      }
    }
    // cout << "\n";

    return g == h;
  }

  vector<Tile>::iterator find_last_points_to(vector<Tile>::iterator x){
    for(auto y = x - 1; y > this->grid.begin(); y--){
      // if it points to x and x points to it, return it
      if(find(y->points_to.begin(), y->points_to.end(), x->pos) != y->points_to.end() && find(x->points_to.begin(), x->points_to.end(), y->pos) != x->points_to.end()){
        return y;
      }
    }
    return this->grid.begin();
  }
};



// first solve strategy
void solve(Grid& grid){
  int m = 0;
  cout << "Solving...\n--------\n";
  auto f = [&](){
    for(auto x = grid.begin() + m; x != grid.end(); x++){
      // cout << grid;

      bool valid = x->valid();
      bool constrained = grid.validate_to(x);

      // cout << x->pos.first << "," << x->pos.second << "\n";
      // cout << valid << " " << constrained << "\n";

      if(x->exhausted()){
        if(x == grid.begin()){
          end();
        }
        // cout << "backtrack 1\n";
        m = x - grid.begin() - 1;
        x->reset();
        (x-1)->rotate();
        return true;
      }

      if(x->empty()){
        continue;
      }

      // if not valid or constraints not met, rotate
      if(!valid || !constrained){
        // cout << "not constrained\n";
        int rot = -1;
        while(!(valid && constrained) && !x->exhausted()){x->rotate(); valid = x->valid(); constrained = grid.validate_to(x); rot+=1;}

        // no usable position found, so backtrack
        if(!valid || !constrained){
          // backup
          while(rot){
            x->reverse();
            rot--;
          }

          // cout << "backtracking\n";

          // nearest constraining tile
          auto new_it = grid.find_last_points_to(x);
          // cout << "new_it = " << new_it->pos.first << "," << new_it->pos.second << "\n";
          // cout << "diff " << new_it - grid.begin() << "\n";

          if(new_it - grid.begin() == 0){
            // cout << "backtracking 1\n";
            m = x - grid.begin() - 1;
            x->reset();
            (x-1)->rotate();
            return true;
          }

          // set move to tile before constraint
          m = new_it - grid.begin() - 1;

          // reset tiles after constraint
          for(auto y = new_it + 1; y == x; y++){
            y->reset();
          }

          // rotate constraint
          new_it->rotate();

          return true;
        }
      }
    }
    return false;
  };

  while(f());

  cout << "solved\n\n";
  cout << grid << endl;
  exit(0);
}


int main(){
  Grid grid = Grid(cin);

  cout << "Input:\n";
  cout << grid << "\n";

  solve(grid);

  return 0;
}
