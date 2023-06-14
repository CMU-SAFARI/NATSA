/*++++++++
Written by Yan Zhu, Jan 2018.

This is SCRIMP++.

Details of the SCRIMP++ algorithm can be found at:
Yan Zhu, Chin-Chia M.Yeh, Zachary Zimmerman, Kaveh Kamgar and Eamonn Keogh,
"Solving Time Series Data Mining Problems at Scale with SCRIMP++", submitted to KDD 2018.

Usage: >> scrimpplusplus InputFileName SubsequenceLength stepsize
InputFileName: Name of the time series file
SubsequenceLength: Subsequence length m
stepsize: Step size ratio s/m. For all the experiments in the paper, stepsize is always set as 0.25.

example input:
>> scrimpplusplus ts_1000.txt 50 0.25

example output:
The code will generate two outputs.
SCRIMP_PLUS_PLUS_New_PreSCRIMP_MatrixProfile_and_Index_50_ts_1000.txt          This is the approximate matrix profile and matrix profile index generated after PreSCRIMP.
SCRIMP_PLUS_PLUS_New_MatrixProfile_and_Index_50_ts_1000.txt                    This is the final/exact matrix profile and matrix profile index, generated when the whole algorithm (PreSCRIMP+SCRIMP) is completed.

The first column of the output file is the matrix profile value.
The second column of the output file is the matrix profile index.
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <vector>
#include <algorithm>
#include <string.h>
#include <sstream>
#include <chrono>
#include <omp.h>
#include "mprofile.h"
#include "/path/to/zsimramulator/misc/hooks/zsim_hooks.h"


int numThreads, exclusionZone;
int windowSize, timeSeriesLength, ProfileLength;
int* profileIndex, *profileIndex_tmp;
DTYPE *AMean, *ASigma, *profile, *profile_tmp;
std::vector<int> idx;
std::vector<DTYPE> A;
int completion_factor = 1;


void preprocess()
{
  DTYPE* ACumSum   = new DTYPE[timeSeriesLength];
  DTYPE* ASqCumSum = new DTYPE[timeSeriesLength];
  DTYPE* ASum      = new DTYPE[ProfileLength];
  DTYPE* ASumSq    = new DTYPE[ProfileLength];
  DTYPE* ASigmaSq  = new DTYPE[ProfileLength];

  AMean  = new DTYPE[ProfileLength];
  ASigma = new DTYPE[ProfileLength];

  ACumSum[0]   = A[0];
  ASqCumSum[0] = A[0] * A[0];

  for (int i = 1; i < timeSeriesLength; i++)
  {
    ACumSum[i]   = A[i] + ACumSum[i - 1];
    ASqCumSum[i] = A[i] * A[i] + ASqCumSum[i - 1];
  }

  ASum[0] = ACumSum[windowSize - 1];
  ASumSq[0] = ASqCumSum[windowSize - 1];

  for (int i = 0; i < timeSeriesLength - windowSize; i++)
  {
    ASum[i + 1]   = ACumSum[windowSize + i] - ACumSum[i];
    ASumSq[i + 1] = ASqCumSum[windowSize + i] - ASqCumSum[i];
  }

  for (int i = 0; i < ProfileLength; i++)
  {
    AMean[i] = ASum[i]/ windowSize;
    ASigmaSq[i] = ASumSq[i] / windowSize - AMean[i] * AMean[i];
    ASigma[i] = sqrt(ASigmaSq[i]);
  }

  delete ACumSum;
  delete ASqCumSum;
  delete ASum;
  delete ASumSq;
  delete ASigmaSq;
}

void scrimp()
{
  #pragma omp parallel
  {
    DTYPE distance, windowSizeDTYPE;
    DTYPE *distances, *lastzs;
    int diag, my_offset, i, j, ri;
    DTYPE lastz;
    windowSizeDTYPE = (DTYPE) windowSize;
    my_offset = omp_get_thread_num() * ProfileLength;

    #pragma omp for schedule(dynamic)

    for (ri = 0; ri < idx.size() / completion_factor; ri++)
    {

      //select a diagonal
      diag = idx[ri];
      lastz = 0;

      //calculate the dot product of every two time series values that ar diag away
      #pragma omp simd
      for (j = diag; j < windowSize + diag; j++)
      {
        lastz += A[j] * A[j-diag];
      }

      //j is the column index, i is the row index of the current distance value in the distance matrix
      j = diag;
      i = 0;

      //evaluate the distance based on the dot product
      distance = 2 * (windowSizeDTYPE - (lastz - windowSizeDTYPE * AMean[j] * AMean[i]) / (ASigma[j] * ASigma[i]));

      //update matrix profile and matrix profile index if the current distance value is smaller
      if (distance < profile_tmp[my_offset + j])
      {
        profile_tmp[my_offset + j]     = distance;
        profileIndex_tmp [my_offset+j] = i;
      }

      if (distance < profile_tmp[my_offset + i])
      {
        profile_tmp[my_offset + i]       = distance;
        profileIndex_tmp [my_offset + i] = j;
      }

      i = 1;
      for(j = diag + 1; j < ProfileLength; j++)
      {
        lastz += (A[ j + windowSize - 1] * A[i + windowSize - 1]) - (A[j - 1] * A[i - 1]);
        distance =  2 * (windowSizeDTYPE - (lastz -  AMean[j]  * AMean[i] * windowSizeDTYPE) / (ASigma[j] * ASigma[i]));

        if (distance < profile_tmp[my_offset + j])
        {
          profile_tmp[my_offset + j] = distance;
          profileIndex_tmp [my_offset+ j] = i;
        }

        if (distance < profile_tmp[my_offset + i])
        {
          profile_tmp[my_offset + i] = distance;
          profileIndex_tmp[my_offset + i] = j;
        }

        i++;
      }
    }

    #pragma omp barrier

    // Reduce the (partial) result
    DTYPE min_distance;
    int min_index;

    #pragma omp for schedule(static)
    for (int colum = 0; colum < ProfileLength; colum++)
    {
      min_distance = std::numeric_limits<DTYPE>::infinity();
      min_index = 0;

      for(int row = 0; row < numThreads; row++)
      {
        if(profile_tmp[colum + (row * ProfileLength)] < min_distance)
        {
          min_distance = profile_tmp[colum + (row * ProfileLength)];
          min_index    = profileIndex_tmp[colum + (row * ProfileLength)];
        }
      }

      profile[colum]      = min_distance;
      profileIndex[colum] = min_index;
    }

    #pragma omp barrier
  }

  delete(AMean);
  delete(ASigma);
  delete(profile_tmp);
  delete(profileIndex_tmp);
}

int main(int argc, char* argv[])
{
  bool sequentialDiags = false;
  // Creation of time meassure structures
  std::chrono::high_resolution_clock::time_point tprogstart, tstart, tend;
  std::chrono::duration<double> time_elapsed;

  // Set window size
  windowSize = atoi(argv[2]);

  // Set the exclusion zone
  exclusionZone = (int) (windowSize * 0.25);

  // Set the thread number
  numThreads = atoi(argv[3]);
  omp_set_num_threads(numThreads);

  // Set computational order
  if(argc > 4)
  sequentialDiags = (strcmp(argv[4], "-s") == 0);

  if(!sequentialDiags && argc > 4)
  completion_factor = atoi(argv[4]);

  // Display info through console
  std::cout << std::endl;
  std::cout << "############################################################" << std::endl;
  std::cout << "///////////////////////// SCRIMP ///////////////////////////" << std::endl;
  std::cout << "############################################################" << std::endl;
  std::cout << std::endl;
  std::cout << "[>>] Reading File..." << std::endl;

  /* Read time series file */
  tstart = std::chrono::high_resolution_clock::now();
  tprogstart = tstart;

  std::stringstream outfilename_num;
  outfilename_num << windowSize;
  std::string outfilenamenum = outfilename_num.str();
  std::string inputfilename  = argv[1];
  std::string outfilename = "SCRIMP_PLUS_PLUS_New_MatrixProfile_and_Index_" + outfilenamenum + "_" + inputfilename;

  loadTimeSeriesFromFile(inputfilename, A, timeSeriesLength);

  tend = std::chrono::high_resolution_clock::now();
  time_elapsed = tend - tstart;
  std::cout << "[OK] Read File Time: " << std::setprecision(std::numeric_limits<double>::digits10 + 2) << time_elapsed.count() << " seconds." << std::endl;

  // Set Matrix Profile Length
  ProfileLength = timeSeriesLength - windowSize + 1;

  // Display info through console
  std::cout << std::endl;
  std::cout << "------------------------------------------------------------" << std::endl;
  std::cout << "************************** INFO ****************************" << std::endl;
  std::cout << std::endl;
  std::cout << " Time series length: " << timeSeriesLength << std::endl;
  std::cout << " Window size:        " << windowSize       << std::endl;
  std::cout << " Exclusion zone:     " << exclusionZone    << std::endl;
  std::cout << " Profile length:     " << timeSeriesLength << std::endl;
  std::cout << " Number of threads:  " << numThreads       << std::endl;
  std::cout << " Sequential order:   ";
  if(sequentialDiags) std::cout << "true" << std::endl;
  else std::cout << "false" << std::endl;
  std::cout << " Completion factor:  " << (double) 100 / completion_factor  << " %"<<std::endl;
  std::cout << std::endl;
  std::cout << "------------------------------------------------------------" << std::endl;
  std::cout << std::endl;

  // Preprocess, statistics, get the mean and standard deviation of every subsequence in the time series
  std::cout << "[>>] Preprocessing..." << std::endl;
  tstart = std::chrono::high_resolution_clock::now();

  preprocess();

  tend = std::chrono::high_resolution_clock::now();
  time_elapsed = tend - tstart;
  std::cout << "[OK] Preprocess Time:         " << std::setprecision(std::numeric_limits<double>::digits10 + 2) << time_elapsed.count() << " seconds." << std::endl;

  //Initialize Matrix Profile and Matrix Profile Index
  std::cout << "[>>] Initializing Profile..." << std::endl;
  tstart = std::chrono::high_resolution_clock::now();

  profile          = new DTYPE[ProfileLength];
  profileIndex     = new int[ProfileLength];
  profile_tmp      = new DTYPE[ProfileLength * numThreads];
  profileIndex_tmp = new int[ProfileLength * numThreads];

  for (int i=0; i<ProfileLength*numThreads; i++) profile_tmp[i] = std::numeric_limits<DTYPE>::infinity();

  tend = std::chrono::high_resolution_clock::now();
  time_elapsed = tend - tstart;
  std::cout << "[OK] Initialize Profile Time: " << std::setprecision(std::numeric_limits<DTYPE>::digits10 + 2) << time_elapsed.count() << " seconds." << std::endl;

  // Random shuffle the diagonals
  idx.clear();
  for (int i = exclusionZone+1; i < ProfileLength; i++)
  idx.push_back(i);

  if(!sequentialDiags)
  std::random_shuffle(idx.begin(), idx.end());

  zsim_roi_begin();
  /******************** SCRIMP ********************/
  std::cout << "[>>] Performing SCRIMP..." << std::endl;
  tstart = std::chrono::high_resolution_clock::now();
  zsim_PIM_function_begin();
  scrimp();
  zsim_PIM_function_end();
  tend = std::chrono::high_resolution_clock::now();
  time_elapsed = tend - tstart;
  std::cout << "[OK] SCRIMP Time:             " << std::setprecision(std::numeric_limits<DTYPE>::digits10 + 2) << time_elapsed.count() << " seconds." << std::endl;
  zsim_roi_end();

  // Save profile to file
  std::cout << "[>>] Saving Profile..." << std::endl;
  tstart = std::chrono::high_resolution_clock::now();

  saveProfileToFile(outfilename.c_str(), profile, profileIndex, timeSeriesLength, windowSize);

  tend = std::chrono::high_resolution_clock::now();
  time_elapsed = tend - tstart;
  std::cout << "[OK] Save Profile Time:       " << std::setprecision(std::numeric_limits<DTYPE>::digits10 + 2) << time_elapsed.count() << " seconds." << std::endl;

  // Calculate total time
  time_elapsed = tend - tprogstart;
  std::cout << "[OK] Total Time:              " << std::setprecision(std::numeric_limits<DTYPE>::digits10 + 2) << time_elapsed.count() << " seconds." << std::endl;
  std::cout << std::endl;

  delete profile;
  delete profileIndex;
}
