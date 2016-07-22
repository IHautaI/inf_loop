#include <future>
#include <boost/program_options.hpp>
#include "lib/Grid.hpp"
namespace po = boost::program_options;
using namespace std;


void end(){
  cout << "No Solution\n";
  exit(0);
}

// first solve strategy
// for each idx, move set (just rotation counts < 4)
// find valid configs for Tile at idx with that move set
// make futures with (m+1, g) and wait for them
// toggling between async launch and deferred launch at specified # async calls
void solve(Grid& grid, int threads){
  int current_threads = 0;

  std::function<vector<pair<std::launch, vector<int>>>(std::launch, int, Grid, vector<int>)> f = [&](std::launch policy, int idx, Grid g, vector<int> moves){
    vector<pair<std::launch, vector<int>>> ret;
    auto it = g.begin() + idx;

    if(it < g.end() - 1){
      vector<future<vector<pair<std::launch,vector<int>>>>> futures; // result + moves

      // next move
      moves.push_back(0);
      // iterate over possibilities
      for(auto m = it->moves.begin(); m < it->moves.end(); m++){
        moves[idx] = m - it->moves.begin();

        // if the move is valid
        if(it->valid() && g.validate_to(it)){
          auto launch = (current_threads < threads)? std::launch::async : std::launch::deferred;
          if(launch == std::launch::async){
            current_threads++;
          }
          // create future
          futures.push_back(std::async(launch, f, launch, idx + 1, g, moves));
        }
        it->rotate();
      }

      // now check futures
      for(auto& x : futures){
        auto y = x.get();
        for(auto& z : y){
            ret.push_back(z);
          if(z.first == std::launch::async){
            threads--;
          }
        }
      }
    }

    if(it == g.end() - 1){
      for(auto m = it->moves.begin(); m < it->moves.end(); m++){
        moves[idx] = m - it->moves.begin();
        // valid last move?
        if(it->valid() && g.validate_to(it)){
          // add to returns
          ret.push_back(make_pair(policy, moves));
        }
        it->rotate();
      }
    }
    return ret;
  };

  auto solutions = f(std::launch::deferred, 0, grid, vector<int>());

  // iterate over solutions
  int i = 0;
  for(auto x : solutions){
    // reset grid
    for(auto y = grid.grid.begin(); y < grid.grid.end(); y++){
      y->reset();
    }
    // iterate through moves, applying to grid and printing steps
    cout << "\nSolution " << i << "\n-----------------\n\n";
    for(auto it = grid.begin(); it < grid.end(); it++){
      it->rotate(x.second.at(it-grid.begin()));
      cout << x.second.at(it-grid.begin()) << " rotations\n";
      cout << grid << "\n\n";
    }
    i++;

    cout << "solved\n\n";
    cout << grid << "\n\n";
    cout << "--------------------\n\n";
  }

  exit(0);
}


int main(int argc, char* argv[]){
  int threads = 2;

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("threads", po::value<int>(), "target # threads to use");

  po::variables_map vm;
  po::store(po::parse_command_line(argc,argv,desc), vm);
  po::notify(vm);

  if(vm.count("help")){
    cout << desc << "\n";
  }
  if(vm.count("threads")){
    threads = vm["threads"].as<int>();
  }


  Grid grid = Grid(cin);

  cout << "Input:\n";
  cout << grid << "\n";

  solve(grid, threads);

  return 0;
}
