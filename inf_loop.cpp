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
  bool rotate(){
    if(this->rotations < 3){
      this->state = this->state & 8 ? ((15 & (this->state << 1)) | 1) : (15 & (this->state << 1));
      this->rotations++;
      this->update();
    }
    return this->rotations < 3;
  }

  bool reverse(){
    if(this->rotations > 0){
      this->state = this->state & 1 ? ((15 & (this->state >> 1)) | 8) : (15 & (this->state >> 1));
      this->rotations--;
      this->update();
    }
    return this->rotations > 0;
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

  bool done(){
    return this->rotations == 3;
  }

  friend ostream& operator<<(ostream& stream, const Tile& t){
    stream << t.state;
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
      stream << x << " " ;
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
    cout << "t state "<< t->state;

    // not needed, just for logging process
    for(auto x = t->points_to.begin(); x != t->points_to.end(); x++){
      cout << ", points_to " << x->first << "," << x->second;
    }

    // up to t, get all tiles who point to t
    for(auto x = this->grid.begin(); x < t; x++){
      cout << "\n";
      cout << x->pos.first << "," << x->pos.second << " state " << x->state <<", points_to ";
      for(auto& y: x->points_to){
        cout << y.first << "," << y.second << " ";
        if(y == t->pos){
          g.insert(x->pos);
        }
      }

      // get all tiles t points to that are before it
      if(find(t->points_to.begin(), t->points_to.end(), x->pos) != t->points_to.end()){
        h.insert(x->pos);
      }
    }
    cout << "\n";

    return g == h;
  }

  vector<Tile>::iterator find_first_points_to(vector<Tile>::iterator x){
    for(auto y = this->grid.begin(); y != this->grid.end(); y++){
      if(find(y->points_to.begin(), y->points_to.end(), x->pos) != y->points_to.end()){
        return y;
      }
    }
    return this->grid.end();
  }
};



// first solve strategy
void solve(Grid& grid){
  int m = 0;

  auto f = [&](){

    for(auto x = grid.begin() + m; x != grid.end(); x++){
      cout << "---------\n";
      cout << grid << "\n";
      cout << "checking t = " << x->pos.first << "," << x->pos.second << "\n";
      cout << "---------\n";

      // no more rotations? done
      if(x->done()){
        end();
      }
      // empty? move on
      if(x->empty()){
        cout << "Tile validated\n";
        continue;
      }

      // if everything valid so far, move on
      if(!(grid.validate_to(x) && x->valid())){
        cout << "invalid\n";

        auto n = x->rotations < 3;
        auto k = x->valid();
        auto r = grid.validate_to(x);
        // rotate until valid, or you can't anymore
        while(n && !(k && r)){n = x->rotate(); cout << "rotating to state " << x->state << "\n\n"; k = x->valid(); r = grid.validate_to(x);}

        // if not valid
        if(!x->valid()){
          cout << "Tile not valid, backtrack\n";
          cout << "searching for constraints\n";
          auto n = true;
          // rotate back until a valid constrained position is found
          while(n && !x->valid() && !grid.validate_to(x)){n = x->reverse();cout << "reversing to state " << x->state << "\n";}

          cout << x->valid() << " " << grid.validate_to(x) << "\n";
          // if not found, done
          if(!x->valid() || !grid.validate_to(x)){
            cout << "no constraints found\n";
            end();
          }

          // otherwise backtrack to constraint
          auto new_it = grid.find_first_points_to(x);

          // if constraint found
          if(new_it != grid.grid.end()){
            cout << "constraint found\n";
            m = new_it - grid.grid.begin();
            if(m < 0){end();}
            // reset all after new start point
            for(auto y = grid.begin() + m + 1; y!= grid.end(); y++){
              y->reset();
            }
            // rotate new start point
            (grid.begin() + m)->rotate();

            return true;
          }
          cout << "no constraints found\n";
          end();
        }

        // if valid, but constrained, backtrack to constraining point
        if(!grid.validate_to(x)){
          cout << "constraint unsatisfied, backtrack\n";
          cout << "searching for constraint\n";
          auto new_it = grid.find_first_points_to(x);

          // if constraint found
          if(new_it != grid.grid.end()){
            cout << "constraint found\n";
            m = new_it - grid.grid.begin();

            // reset all after new start point
            for(auto y = grid.begin() + m + 1; y != grid.end(); y++){
              y->reset();
            }
            // rotate new start point
            (grid.begin() + m)->rotate();

            return true;
          }
          cout << "no constraint found\n";
          end();
        }
      }
      cout << "Tile validated\n";
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
