#include <iostream>
#include "utils.h"
#include <string>
#include <stdio.h>

// Function calling the kernel to operate
void rgba_to_grey(uchar4 * const d_rgbaImage,
                  unsigned char* const d_greyImage, 
                  size_t numRows, size_t numCols,
                  int aBlockSize, int aGridSize);

//include the definitions of the above functions for this homework
#include "preprocess.cpp"

int main(int argc, char **argv) {
  // rgbaImage => imagen original
  // h_greyImage => imagen reducida
  uchar4        *h_rgbaImage, *d_rgbaImage;
  unsigned char *h_greyImage, *d_greyImage;

  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);

  std::string input_file;
  std::string output_file;
  

  //make sure the context initializes ok
  checkCudaErrors(cudaFree(0));

  input_file = std::string(argv[1]);
  output_file = "output.png";

  int threads;
  int blocks;

  sscanf(argv[2], "%i", &threads);
  sscanf(argv[3], "%i", &blocks);

  //load the image and give us our input and output pointers
  preProcess(&h_rgbaImage, &h_greyImage, &d_rgbaImage, &d_greyImage, input_file);

  //call the cuda code
  cudaEventRecord(start);
  rgba_to_grey(d_rgbaImage, d_greyImage, numRows(), numCols(), threads, blocks);
  cudaEventRecord(stop);
  cudaEventSynchronize(stop);

  float milliseconds = 0;
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("Time elapsed: %lf\n", milliseconds);

  //size_t numPixels = numRows()*numCols();
  //cols => 852
  //rows => 480
  size_t numPixels = 480*852;
  checkCudaErrors(cudaMemcpy(h_greyImage, d_greyImage, sizeof(unsigned char) * numPixels * 3, cudaMemcpyDeviceToHost));

  /* Output the grey image */
  cv::Mat output(480, 852, CV_8UC3, (void*)h_greyImage);
  // Open the window
  // cv::namedWindow("to_bw");
  // Display the image m in this window
  // cv::imshow("to_bw", output);
  // cvWaitKey (0);
  // cvDestroyWindow ("to_bw");
  //output the image
  cv::imwrite(output_file.c_str(), output);

  /* Cleanup */
  cudaFree(d_rgbaImage__);
  cudaFree(d_greyImage__);

  return 0;
}
