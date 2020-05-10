# yalsa
Yet Another Loop Schedule Analysis


This is a very simple loop schedule analysis for teaching purposes.  
It should be seen as a proof of concept. : )

# Purpose

Model the memory bandwidth behavior of a given loop schedule.

Limitations (yes, there are many many):
* Arrays may only be index by the sum of variables (no striding)
* Loops iterate over a single dimension
* Computations are fully commutative
* Loopnests are "perfect" -- computation only at the innermost loop
* No control flow beyond loops
* Bandwidth computations are insensitive to data layout (no cacheline penalties)
* Output arrays are treated the same as input arrays, this overestimates data slightly

Even with the above, analyzing reuse and bandwidth requirements for many 
dense matrix operations should be possible.

# Notes on usage
* Currently, there are a fixed set of variable dimensions, more can be added
* Loopnests are the unit of abstraction, and have:
  - Problem Dimensions
  - Set of arrays accessed in each iteration
    * Arrays specified by which dimensions they are coupled with
    * Convolution pattern is described by array dimension coupling with two variables
  - Set of loop levels
    * Loop levels are defined by variable and extent 
    * Loopnests are defined from outermost to innermost loop
    * Tiling may be specified by duplicate loops over the same dimension
    * Tiling is when extent < dimension size
    * Extent of each dimension must be the same or decrease from outer to innermost loops

