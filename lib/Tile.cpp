#include "Tile.hpp"

  // updates points_to set using current state
void Tile::update(){
  this->points_to.clear();
  int state = *(this->state);
  // points up, not at the top
  if((UP & state) != 0 && this->pos.first != 0){
    this->points_to.insert(make_pair(this->pos.first - 1, this->pos.second));
  }
  // points down, not at the bottom
  if((DOWN & state) != 0 && this->pos.first != this->end.first){
    this->points_to.insert(make_pair(this->pos.first + 1, this->pos.second));
  }
  // points left, not on left edge
  if((LEFT & state) != 0 && this->pos.second != 0){
    this->points_to.insert(make_pair(this->pos.first, this->pos.second - 1));
  }
  // points right, not on right edge
  if((RIGHT & state) != 0 && this->pos.second != this->end.second){
    this->points_to.insert(make_pair(this->pos.first, this->pos.second + 1));
  }
}

void Tile::make_moves(){
  int st = this->start;
  do {
    this->moves.push_back(st);
    // rotate
    st = (8 & st) ? ((15 & (st << 1)) | 1) : (15 & (st << 1));
  } while(st != this->start);
  this->state = this->moves.begin();
}

bool Tile::operator==(const Tile other){
  return this->pos == other.pos;
}

void Tile::rotate(){
  if(this->state + 1 < moves.end()){
    this->state += 1;
  }
}

void Tile::rotate(int& i){
  if(this->state + i < this->moves.end()){
    this->state += i;
  }
}

Tile::Tile(int x, pair<int,int> y):start(x),pos(y){this->make_moves();}

// copy cstr
Tile::Tile(const Tile& o){
  this->start = o.start;
  this->pos = o.pos;
  this->end = o.end;
  this->make_moves();
  this->state = this->moves.begin() + (o.state - o.moves.begin());
  this->update();
}

void Tile::reset(){
  this->state = moves.begin();
  this->update();
}

bool Tile::empty(){
  return !(0 | this->start);
}

ostream& operator<<(ostream& stream, Tile const& t){
  stream << Tile::uc.at(*(t.state));
  return stream;
}

void Tile::set_end(pair<int,int> r){
  this->end = r;
  this->update();
}

bool Tile::valid(){
  int state = *(this->state);
  int width = this->end.first;
  int length = this->end.second;
  int i = get<0>(this->pos);
  int j = get<1>(this->pos);
  return !((i == 0 && (state & UP) != 0) || (i == width && (state & DOWN) != 0) || (j == 0 && (state & LEFT) != 0) || (j == length && (state & RIGHT) != 0));
}

const map<int,string> Tile::uc = {{0,"o"}, {1,"\u2575"}, {2,"\u2576"},{3,"\u2514"}, {4,"\u2577"},
            {5,"\u2502"}, {6,"\u250C"}, {7,"\u251C"}, {8,"\u2574"},
            {9,"\u2518"}, {10,"\u2500"}, {11,"\u2534"}, {12, "\u2510"},
            {13, "\u2524"}, {14,"\u252C"},{15,"\u253C"}};
