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

  void update(){
    if(UP & this->state && this->pos.first != 0){
      points_to.push_back(make_pair(this->pos.first - 1, this->pos.second));
    }
    if(DOWN & this->state && this->pos.first != this->end.first){
      points_to.push_back(make_pair(this->pos.first + 1, this->pos.second));
    }
    if(LEFT & this->state && this->pos.second != 0){
      points_to.push_back(make_pair(this->pos.first, this->pos.second - 1));
    }
    if(RIGHT & this->state && this->pos.second != this->end.second){
      points_to.push_back(make_pair(this->pos.first, this->pos.second + 1));
    }
  }

public:
  int state;
  int rotations;
  pair<int,int> pos;
  vector<pair<int,in>> points_to;

  // up-right-down-left, clockwise rotation
  // returns true if it can continue rotating
  bool rotate(){
    bool carry = this->state & 8;
    this->state = 15 & (this->state << 1);
    if(carry){
      this->state = this->state | 1;
    }
    this->rotations++;
    this->update();
    return this->rotations < 3;
  }

  void reset(){
    this->state = this->start;
    this->rotations = 0;
  }

  Tile(int x, pair<int,int> y):start(x),pos(y){state = start; this->update();}

  // copy cstr
  Tile(const Tile& o){
    start = o.start;
    state = o.start;
    rotations = 0;
  }

  void reset(){
    this->state = start;
    this->rotations = 0;
    this->update();
  }

  bool empty(){
    return 0 | this->state;
  }

  friend ostream& operator<<(ostream& stream, const Tile& t){
    stream << t.state;
    return stream;
  }

  bool valid(pair<int,int> pos, int width, int length){
    int i = get<0>(pos);
    int j = get<1>(pos);
    return !((i == 0 && this->state & UP) || (i == width - 1 && this->state & DOWN) || (j == 0 && this->state & LEFT) || (j == length - 1 && this->state & RIGHT));
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
    for(auto x : grid){
      if(x.pos.second == 0){
        stream << "\n";
      }
      stream << x << " ";
    }
    stream << "\n";

    return stream;
  }

  Grid(istream& in){
    // create grid from stdin tsv
    vector<Tile> gr;

    stringstream ss;
    string line;

    int i = 0;
    // read in lines one-by-one into line
    while(getline(in, line)){
      // setup stream from line
      ss.clear();
      ss.str("");
      ss << line;

      // extract tiles from stringstream, put in map
      int y;
      int j = 0;
      vector<int> check;
      while(ss >> y){
        Tile t(y, make_pair(i,j));
        gr.push_back(t);
        j++;
      }
      check.push_back(j);
      i++;
    }

    // validate size, should be rectangular
    for(auto& x: check){
      if(x != check[0]){
        cerr << "Invalid Input, not rectangular.  Exiting" << endl;
        exit(1);
      }
    }

    // set values
    this->grid = gr;
    this->width = i;
    this->length = target;
    cout << "valid shape input\n";
    cout << "width: " <<this->width << "\n";
    cout << "length: " << this->length << "\n";
  }

  bool validate_to(Tile& t){
    bool ret = true;
    map<pair<int,int>, pair<int,int>> g;
    for(auto& x = this->begin(); x != t; x++){
      for(auto& y: x->points_to){
        g[x->pos] = y;
      }
    }

    for(auto& z : g){
      if(g.find(swap_pair(z)) == g.end()){
        ret = false;
      }
    }
    return ret;
  }
};



// first solve strategy
void solve(Grid& grid){

}


int main(){
  Grid grid = Grid(cin);
  Edge edge;

  cout << "Input:\n";
  cout << grid;

  // while not solved, solve it...
  while(!edge.complete(grid)){
    solve(grid);
    cout << "\n";
    cout << grid;
  }
  cout << "\n\n";
  // print solved grid
  cout << "solved:\n";
  cout << grid;

  return 0;
}
