#include <stdio.h>
#include "cs1300bmp.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "Filter.h"

using namespace std;

#include "rdtsc.h"

inline int Filter::getDivisor(){return divisor;}
inline int Filter::getSize(){return dim;}
inline int Filter::get(int r, int c){return data[ r * dim + c ];}

//
// Forward declare the functions
//
Filter * readFilter(string filename);
double applyFilter(Filter *filter, cs1300bmp *input, cs1300bmp *output);

int
main(int argc, char **argv)
{

  if ( argc < 2) {
    fprintf(stderr,"Usage: %s filter inputfile1 inputfile2 .... \n", argv[0]);
  }

  //
  // Convert to C++ strings to simplify manipulation
  //
  string filtername = argv[1];

  //
  // remove any ".filter" in the filtername
  //
  string filterOutputName = filtername;
  string::size_type loc = filterOutputName.find(".filter");
  if (loc != string::npos) {
    //
    // Remove the ".filter" name, which should occur on all the provided filters
    //
    filterOutputName = filtername.substr(0, loc);
  }

  Filter *filter = readFilter(filtername);

  double sum = 0.0;
  int samples = 0;

  for (int inNum = 2; inNum < argc; inNum++) {
    string inputFilename = argv[inNum];
    string outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    struct cs1300bmp *input = new struct cs1300bmp;
    struct cs1300bmp *output = new struct cs1300bmp;
    int ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);

    if ( ok ) {
      double sample = applyFilter(filter, input, output);
      sum += sample;
      samples++;
      cs1300bmp_writefile((char *) outputFilename.c_str(), output);
    }
    delete input;
    delete output;
  }
  fprintf(stdout, "Average cycles per sample is %f\n", sum / samples);

}

class Filter *
readFilter(string filename)
{
  ifstream input(filename.c_str());

  if ( ! input.bad() ) {
    int size = 0;
    input >> size;
    Filter *filter = new Filter(size);
    int div;
    input >> div;
    filter -> setDivisor(div);
    for (int i=0; i < size; i++) {
      for (int j=0; j < size; j++) {
	int value;
	input >> value;
	filter -> set(i,j,value);
      }
    }
    return filter;
  } else {
    cerr << "Bad input in readFilter:" << filename << endl;
    exit(-1);
  }
}


double
applyFilter(class Filter *filter, cs1300bmp *input, cs1300bmp *output)
{

    long long cycStart, cycStop;

    cycStart = rdtscll();

    output -> width = input -> width;
    output -> height = input -> height;
    const short int f = filter->getDivisor();
    const short int w = input->width;
    const short int h = input->height;

    for(int row = 1; row < h - 1 ; row = row + 1) {
        for(int col = 1; col < w - 1; col = col + 1) {
            int tempRed = 0;
            int tempGreen = 0;
            int tempBlue = 0;
            
                tempRed += (input -> color[row - 1][col - 1][0] * filter -> get(0, 0) )
                        + (input -> color[row - 1][col][0] * filter -> get(0, 1) )
                        + (input -> color[row - 1][col + 1][0] * filter -> get(0, 2) );
            
                tempRed += (input -> color[row][col - 1][0] * filter -> get(1, 0) )
                        + (input -> color[row][col][0] * filter -> get(1, 1) )
                        + (input -> color[row][col + 1][0] * filter -> get(1, 2) );
            
                tempRed += (input -> color[row + 1][col - 1][0] * filter -> get(2, 0) )
                        + (input -> color[row + 1][col][0] * filter -> get(2, 1) )
                        + (input -> color[row + 1][col + 1][0] * filter -> get(2, 2) );
                
                tempGreen += (input -> color[row - 1][col - 1][1] * filter -> get(0, 0) )
                        + (input -> color[row - 1][col][1] * filter -> get(0, 1) )
                        + (input -> color[row - 1][col + 1][1] * filter -> get(0, 2) );
            
                tempGreen += (input -> color[row][col - 1][1] * filter -> get(1, 0) )
                        + (input -> color[row][col][1] * filter -> get(1, 1) )
                        + (input -> color[row][col + 1][1] * filter -> get(1, 2) );
            
                tempGreen += (input -> color[row + 1][col - 1][1] * filter -> get(2, 0) )
                        + (input -> color[row + 1][col][1] * filter -> get(2, 1) )
                        + (input -> color[row + 1][col + 1][1] * filter -> get(2, 2) );
                
                tempBlue += (input -> color[row - 1][col - 1][2] * filter -> get(0, 0) )
                        + (input -> color[row - 1][col][2] * filter -> get(0, 1) )
                        + (input -> color[row - 1][col + 1][2] * filter -> get(0, 2) );
                
                tempBlue += (input -> color[row][col - 1][2] * filter -> get(1, 0) )
                        + (input -> color[row][col][2] * filter -> get(1, 1) )
                        + (input -> color[row][col + 1][2] * filter -> get(1, 2) );
                
                tempBlue += (input -> color[row + 1][col - 1][2] * filter -> get(2, 0) )
                        + (input -> color[row + 1][col][2] * filter -> get(2, 1) )
                        + (input -> color[row + 1][col + 1][2] * filter -> get(2, 2) );
                
            tempRed /= f;
            
            tempGreen /= f;
            
            tempBlue /= f;
            
            if ( tempRed  < 0 ) {
                tempRed = 0;
            }
            if ( tempRed  > 255 ) { 
                tempRed = 255;
            }
            if ( tempGreen  < 0 ) {
                tempGreen = 0;
            }
            if ( tempGreen  > 255 ) { 
                tempGreen = 255;
            }
            if ( tempBlue  < 0 ) {
                tempBlue = 0;
            }
            if ( tempBlue  > 255 ) { 
                tempBlue = 255;
            }
            
            output -> color[row][col][COLOR_RED] = tempRed;
            output -> color[row][col][COLOR_GREEN] = tempGreen;
            output -> color[row][col][COLOR_BLUE] = tempBlue;
        }
    }

  cycStop = rdtscll();
  double diff = cycStop - cycStart;
  double diffPerPixel = diff / (output -> width * output -> height);
  fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n",
	  diff, diff / (output -> width * output -> height));
  return diffPerPixel;
}
