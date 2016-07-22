#include "Grid.hpp"

vector<Tile> Grid::spiral_sort(int rowsize, vector<Tile>& vec){
  vector<Tile> x;
  peel_tr(vec, x, 0, rowsize, 0, vec.size() / rowsize, rowsize, 0);
  return x;
}

void Grid::peel_tr(vector<Tile>& vec, vector<Tile>& x, int hstart, int hend, int vstart, int vend, int rowsize, int total){
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

void Grid::peel_bl(vector<Tile>& vec, vector<Tile>& x, int hstart, int hend, int vstart, int vend, int rowsize, int total){
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

ostream& operator<<(ostream& stream, Grid const& gr){
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

Grid::Grid(istream& in){
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

bool Grid::validate_to(vector<Tile>::iterator t){
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
