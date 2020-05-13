## ECE259: Mini-Project 2
Due: May 13, 2020
Bryan Bednarski & Matt Nicholas

This C++ code base is adapted from the Yalsa loop scheduler (below), as a simple model of the Titan V GPGPU computational and memory bound times. In this code, the memory bound time (DRAM to cache) and computational bound time (execuation time) are modeled for convolution and matrix multiply kernels. These results are modeled to the theoretical bandwidth and throughput of the system to see how closley this simple model is able to track the runtimes of highly optimized CUDNN kernels through the DeepBench simulation environment from Baidu Research.

### Code adapted from Yalsa: Yet Another Loop Schedule Analysis Tool
https://github.com/PolyArch/yalsa

### Results compared to DeepBench CUDNN kernel execuation times
https://github.com/baidu-research/DeepBench
