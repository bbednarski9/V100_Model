#include "loopnest.hh"

using namespace std;



void Loopnest::get_extent(std::vector<int>& extent, int start_loop_level){
  extent.resize(dims.size(),1);

  for(int j = start_loop_level; j < loops.size(); ++j) {
    int loop_var = loops[j].var;
    extent[loop_var] = max(loops[j].iters,extent[loop_var]);
  }
}

int Loopnest::volume_at_level(Array& arr, int start_loop_level) {
  // compute the "extent" or range of each variable at this level of the loop nest
  // some redundant computation here, but oh well
  std::vector<int> extent;
  get_extent(extent,start_loop_level);

  // compute the volume at this level according to extents
  int volume = 1;
  for(std::vector<int>& arr_dim : arr.coupling) {
    int total_read=0;
    int total_added=0;
    for(int var : arr_dim) {
      total_read+=extent[var];
      total_added++;
    }

    // I think this adjustment is necessary? TODO:check
    if(total_added>1) {
      total_read-=total_added-1;
    }
    volume *= total_read;
  }

  return volume;
}

int Loopnest::iters_at_level(int start_loop_level) {
  std::vector<int> extent;
  get_extent(extent,start_loop_level);

  int iters=1;
  for(int i = 0; i < extent.size(); i++) {
    iters*=extent[i];
  }
  return iters;
}

  // Assume all arrays are in cache
  // datatype_bytes: # bytes in one data item
  // cache_bytes: # bytes in cache
  // return value: # bytes / cycle required
  // iter: iteration number that fits in cache
float Loopnest::bandwidth_for_cache(int datatype_bytes, int cache_bytes,
    int iters_per_cycle, int & lvl) {

  int total_volume;

  for(lvl=0; lvl < loops.size(); ++lvl) {
    total_volume=0;
    for(int a = 0; a < arrays.size(); ++a) {
      total_volume += volume_at_level(arrays[a],lvl);
    }
    total_volume *= datatype_bytes;

    if(total_volume < cache_bytes) {
      break;
    }
  }
  int iters = iters_at_level(lvl);

  if(lvl == loops.size()) {
    total_volume=0;
    for(int a = 0; a < arrays.size(); ++a) {
      total_volume += volume_at_level(arrays[a],lvl);
    }
    total_volume*=datatype_bytes;
  }

  float bytes_per_cycle = (float) total_volume / ((float)iters / (float)iters_per_cycle);

  return bytes_per_cycle;
}

void Loopnest::print_volume_analysis() {
  printf("Array Volumes by Loop Level: (in element count)\n");

  printf("%10s","Loop Level");
  for(int a = 0; a < arrays.size(); ++a) {
    printf("%10s",arrays[a].name.c_str());
  }
  printf("\n");


  for(int i = 0; i < loops.size(); ++i) {
    printf("%10d",i);

    for(int a = 0; a < arrays.size(); ++a) {
      printf("%10d",volume_at_level(arrays[a],i));
    }
    printf("\n");
  }

}

void Loopnest::print_bandwidth_analysis() {

  int iters_per_cycle=16;
  int datatype_bytes=2;
  printf("\nAssume %d iters/cycle, datatype size: %d bytes\n",
      iters_per_cycle, datatype_bytes);
  printf("%14s%13s%8s\n","cache size(B)","b/w (B/cyc)","fit_lvl");
  int bytes=16;
  for(int i = 0; i < 20; ++i,bytes=bytes*2) {
    int lvl;
    float bw = bandwidth_for_cache(2,bytes,iters_per_cycle,lvl);
    printf("%14d%13.2f%8d\n",bytes,bw,lvl);
  }
}

// added by bryanbed
int Loopnest::compute_memory_read(int datatype_bytes) {
  int mem_total = 0;
  for(int i = 0; i < arrays.size(); i++){
    int tmp = 1;
    //printf("Array: %s\n", arrays[i].name.c_str());
    for(std::vector<int>& arr_dim : arrays[i].coupling) {
      int dim_limit = 0;
      for(int var : arr_dim) {
        if(dim_limit == 0) {
          //printf("Variable dimension: %d\n", dims[var]);
          tmp *= dims[var]; }
        dim_limit += 1;
      mem_total += tmp;
  } } }
  return mem_total * datatype_bytes;
}

float Loopnest::memory_bound_time() {
  int datatype_bytes=4;
  int memory_bw = 652000; //-> uSec, using 652 GB/sec
  int total_data = compute_memory_read(datatype_bytes);
  float memory_bound_time = (float) total_data / memory_bw; // -> in uSec
  printf("Total Data Size:%d\n",total_data);
  printf("Expected memory bound time:%fuSec\n",memory_bound_time);

  return memory_bound_time;
}