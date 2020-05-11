#include <vector>
#include <stdio.h>
#include <string>

// Variables are given an integer index for simplicity:
#define VarN 0 //batch
#define VarC 1 //input channels
#define VarK 2 //output channels
#define VarX 3 //Width
#define VarY 4 //Height
#define VarS 5 //Filter Row
#define VarR 6 //Filter Column
#define VarCount 7 //Filter Column

//void print_var(int v) {
//  switch(v) {
//    case VarN: printf("N");
//    case VarC: printf("C");
//    case VarK: printf("K");
//    case VarX: printf("X");
//    case VarY: printf("Y");
//    case VarS: printf("S");
//    case VarR: printf("R");
//    default: printf("?");
//  }
//}

using namespace std;

//very simple indexing expression
//assumed that indices are added together
class Array {
public:
  string name;
  vector<vector<int>> coupling;
  Array(string n,vector<vector<int>> c) {
    coupling = c;
    name=n;
  }
};

class Loop {
public:
  int var;
  int iters;
  Loop(int v, int i) {
    var=v;
    iters=i;
  }
};

class Loopnest {
public:
  std::vector<int> dims; // how big is each dimension
  std::vector<Array> arrays;

  std::vector<Loop> loops;

  Loopnest() {
    dims.resize(VarCount);
  }

  void get_extent(std::vector<int>& extent, int start_loop_level);

  int volume_at_level(Array& arr, int start_loop_level);

  int iters_at_level(int start_loop_level);

  float bandwidth_for_cache(int datatype_bytes, int cache_bytes, int iters_per_cycle, int & lvl);

  void print_volume_analysis();

  void print_bandwidth_analysis();

  // added by bryanbed
  int compute_memory_read(int datatype_bytes);

  float memory_bound_time();

  int compute_memory_read_v2(int datatype_bytes);

  float memory_bound_time_v2();

  float comp_bound_time(bool conv);

};
