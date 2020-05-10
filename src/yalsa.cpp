// Yet Another Simple Loop Analysis

#include <assert.h>
#include <getopt.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <math.h>

#include "loopnest.hh"

using namespace std;

void add_example_conv(std::vector<Loopnest>& lns) {
  Loopnest b;
  lns.push_back(b);
  Loopnest& ln = lns.back();

  // This is where the problem parameters are set
  //DianNao -- Conv3
  ln.dims[VarN]=1;
  ln.dims[VarC]=64;
  ln.dims[VarK]=64;
  ln.dims[VarX]=224;
  ln.dims[VarY]=224;
  ln.dims[VarR]=3;
  ln.dims[VarS]=3;

  // Arrays are defined by how they couple with variables.
  // Two variables in a list implies addition (yes, quite a limitation : )

  Array array_i = Array("i",{{VarN},{VarC},{VarY,VarR},{VarX,VarS}});  //Y' X'
  Array array_w = Array("w",{{VarK},{VarC},{VarR},{VarS}});
  Array array_o = Array("o",{{VarN},{VarK},{VarY},{VarX}}); //Y' X'

  ln.arrays.push_back(array_i);
  ln.arrays.push_back(array_w);
  ln.arrays.push_back(array_o);

  // These tiling factors matched what I assumed in the slides, they are not optimal : )
  float Tk = 16; //inner tiling for output channels
  float Tc = 16; //tiling for input channels
  float Tkk = 64; //outer tiling for output channels
  float Tx = 8; //tiling for x dimension
  float Ty = 8; //tiling for y dimension

  // * Loop schedule is defined from outer to inner loops
  //
  // * Loops may only iterate over one dimension of the data (another
  // limitation)
  //
  // * Loops may iterate over a subset of the data by specifying a tile size
  // less than the total extent of the data.
  //
  // * For any given data dimension, tile sizes must be decreasing as you go
  // further inward in the loop.  Otherwise bad things will happen.

  // DianNao Convolution Loop Schedule -- not the idea schedule I must say
  auto& loops = ln.loops;
  auto& dims = ln.dims;

  loops.emplace_back(VarN,dims[VarN]); // Batch loop

  loops.emplace_back(VarY,dims[VarY]); // Spatial Loops
  loops.emplace_back(VarX,dims[VarX]);
  loops.emplace_back(VarK,dims[VarK]); // Output Channel Loop

  loops.emplace_back(VarY,Ty); // Tile Spatial Loop
  loops.emplace_back(VarX,Tx);
  loops.emplace_back(VarK,Tkk); // First Tile Output Channel Loop

  loops.emplace_back(VarS,dims[VarS]); // Filter Loops
  loops.emplace_back(VarR,dims[VarR]);
  loops.emplace_back(VarC,dims[VarC]); // Input Channel Loop
  loops.emplace_back(VarK,Tk); // Second Tile Output Channel Loop
  loops.emplace_back(VarC,Tc); // Tile Input Channel Loop

}

void add_example_mm(std::vector<Loopnest>& lns) {
  Loopnest b;
  lns.push_back(b);
  Loopnest& ln = lns.back();
  ln.dims[VarN]=64;
  ln.dims[VarC]=256;
  ln.dims[VarK]=256;

  Array array_a = Array("a",{{VarN},{VarC}});
  Array array_b = Array("b",{{VarC},{VarK}});
  Array array_c = Array("c",{{VarN},{VarK}});

  ln.arrays.push_back(array_a);
  ln.arrays.push_back(array_b);
  ln.arrays.push_back(array_c);


  int Tn=32;
  int Tc=32;
  int Tk=32;

  ln.loops.emplace_back(VarN,ln.dims[VarN]);
  ln.loops.emplace_back(VarC,ln.dims[VarC]);
  ln.loops.emplace_back(VarK,ln.dims[VarK]);
  //ln.loops.emplace_back(VarN,Tn);
  //ln.loops.emplace_back(VarC,Tc);
  //ln.loops.emplace_back(VarK,Tk);
}




int main(int argc, char* argv[]) {

  std::vector<Loopnest> lns;
  add_example_conv(lns);
  add_example_mm(lns);

  // Do some basic analysis on each:
  printf("\n");
  printf(" ----- Example Conv Analysis ----- \n");
  lns[0].print_volume_analysis();
  lns[0].print_bandwidth_analysis();

  printf("\n");
  printf(" ----- Example MM Analysis ----- \n");
  lns[1].print_volume_analysis();
  lns[1].print_bandwidth_analysis();

  printf("\n");
  printf(" ----- Conv Memory Analysis ----- \n");
  float memory_bound_time_conv = lns[0].memory_bound_time();

  printf("\n");
  printf(" ----- MM Memory Analysis ----- \n");
  float memory_bound_time_mm = lns[1].memory_bound_time();

  return 0;
}
