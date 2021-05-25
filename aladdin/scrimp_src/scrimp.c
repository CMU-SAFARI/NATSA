#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// PROGRAM PARAMETERS
#define TIMESERIESLENGTH 1024
#define WINDOWSIZE 256
#define DIAGONAL_UNROLL 128
#define COMPLETION_REDUCTION 32
#define DTYPE float


void scrimp(DTYPE* tSeries, DTYPE* AMean, DTYPE* ASigma, int ProfileLength,
  int windowSize, int* idx, DTYPE* profile, int* profileIdxs, int exclusionZone, DTYPE * distances, DTYPE * lastzs)
  {

    for (int ri = 0; ri < (ProfileLength - (exclusionZone +1)) /COMPLETION_REDUCTION; ri++)
    {
      //select a random subsequence
      int subseq = idx[ri];

      DTYPE lastz = 0;
      //calculate the dot product of subsequence and query
      dotprod: for (int j = 0; j < windowSize; j++)
      {
        lastz += tSeries[j + subseq]* tSeries[j];
      }

      //evaluate the distance based on the dot product
      DTYPE distance = 2 * (windowSize - (lastz - windowSize * AMean[subseq]
        * AMean[0]) / (ASigma[subseq] * ASigma[0]));

        if (distance < profile[subseq])
        {
          profile[subseq]     = distance;
          profileIdxs[subseq] = 0;
        }
        if (distance < profile[0])
        {
          profile[0]     = distance;
          profileIdxs[0] = subseq;
        }

        int i = 1;

        int jj = subseq + 1;

        while(jj < (ProfileLength - DIAGONAL_UNROLL))
        {

          diagonal1: for(int k = 0; k < DIAGONAL_UNROLL; k++)
          {
            lastzs[k] = (tSeries[k + jj + windowSize - 1] * tSeries[k + i + windowSize - 1]) - (tSeries[k + jj - 1] * tSeries[k + i - 1]);
          }

          lastzs[0] += lastz;

          diagonal2:for(int kkk = 1; kkk < DIAGONAL_UNROLL; kkk++)
          {
            lastzs[kkk] += lastzs[kkk-1];
          }
          lastz = lastzs[DIAGONAL_UNROLL - 1];


          diagonal3:for(int kk = 0; kk < DIAGONAL_UNROLL; kk++)
          {
            distances[kk] =  2 * (windowSize - (lastzs[kk] -  AMean[kk+jj]  * AMean[kk+i] * windowSize) / (ASigma[kk+jj] * ASigma[kk+i]));
          }

          diagonal4:for(int kkkk = 0; kkkk < DIAGONAL_UNROLL; kkkk++)
          {
            if (distances[kkkk] < profile[kkkk + jj])
            {
              profile[kkkk + jj] = distances[kkkk];
              profileIdxs[kkkk +  jj] = i + kkkk;
            }

            if (distances[kkkk] < profile[kkkk +  i])
            {
              profile[kkkk +  i] = distances[kkkk];
              profileIdxs[kkkk + i] = jj + kkkk;
            }
          }
          i+=DIAGONAL_UNROLL;
          jj+=DIAGONAL_UNROLL;
        }
        while(jj < ProfileLength)
        {
          lastz   = lastz + (tSeries[jj + windowSize - 1] * tSeries[i + windowSize - 1]) - (tSeries[jj - 1] * tSeries[i - 1]);
          distance = 2 * (windowSize - (lastz -  AMean[jj]  * AMean[i] * windowSize) / (ASigma[jj] * ASigma[i]));

          if (distance < profile[jj])
          {
            profile[jj] = distance;
            profileIdxs [jj] = i;
          }

          if (distance < profile[i])
          {
            profile[i] = distance;
            profileIdxs[i] = jj;
          }
          i++;
          jj++;
        }
      }
    }

    int main(int argc, char* argv[])
    {
      int windowSize, timeSeriesLength, ProfileLength, exclusionZone;

      DTYPE AMean  [TIMESERIESLENGTH];
      DTYPE ASigma [TIMESERIESLENGTH];

      DTYPE tSeries [TIMESERIESLENGTH];
      DTYPE lastzs [DIAGONAL_UNROLL];
      DTYPE distances [DIAGONAL_UNROLL];
      DTYPE profile [TIMESERIESLENGTH];

      int profileIdxs[TIMESERIESLENGTH];
      int idx[TIMESERIESLENGTH];

      timeSeriesLength = TIMESERIESLENGTH;
      windowSize       = WINDOWSIZE;

      for (int i = 0; i < timeSeriesLength; i++)
      {
        tSeries[i] = ((DTYPE)(rand() % 50)) / 10 + 1;
      }



      // set Matrix Profile Length
      ProfileLength = timeSeriesLength - windowSize + 1;

      exclusionZone = windowSize / 4;

      // preprocess, statistics, get the mean and standard deviation of every subsequence in the time series
      DTYPE ACumSum [TIMESERIESLENGTH];
      ACumSum[0] = tSeries[0];
      for (int ii = 1; ii < timeSeriesLength; ii++)
      ACumSum[ii] = tSeries[ii] + ACumSum[ii - 1];
      DTYPE ASqCumSum[TIMESERIESLENGTH];
      ASqCumSum[0] = tSeries[0] * tSeries[0];
      for (int iii = 1; iii < timeSeriesLength; iii++)
      ASqCumSum[iii] = tSeries[iii] * tSeries[iii] + ASqCumSum[iii - 1];
      DTYPE ASum[TIMESERIESLENGTH];
      ASum[0] = ACumSum[windowSize - 1];
      for (int iiii = 0; iiii < timeSeriesLength - windowSize; iiii++)
      ASum[iiii + 1] = ACumSum[windowSize + iiii] - ACumSum[iiii];
      DTYPE ASumSq[TIMESERIESLENGTH];
      ASumSq[0] = ASqCumSum[windowSize - 1];
      for (int ij = 0; ij < timeSeriesLength - windowSize; ij++)
      ASumSq[ij + 1] = ASqCumSum[windowSize + ij] - ASqCumSum[ij];
      for (int ijj = 0; ijj < ProfileLength; ijj++)
      AMean[ijj] = ASum[ijj] / windowSize;
      DTYPE ASigmaSq[TIMESERIESLENGTH];
      for (int ik = 0; ik < ProfileLength; ik++)
      ASigmaSq[ik] = ASumSq[ik] / windowSize - AMean[ik] * AMean[ik];
      for (int ikk = 0; ikk < ProfileLength; ikk++)
      ASigma[ikk] = sqrt(ASigmaSq[ikk]);

      int starting_counter=exclusionZone+1;
      int ending_counter = ProfileLength -1;

      // Static scheduling
      for (int ig = exclusionZone + 1; ig < ProfileLength; ig+=2)
      {
        //idx[i - (exclusionZone + 1)] = i;
        idx[ig - (exclusionZone + 1)] = starting_counter;
        idx[ig - (exclusionZone)]     = ending_counter;
        starting_counter++;
        ending_counter--;
      }

      for (int igg = 0; igg < ProfileLength; igg++)
      {
        profile[igg]     = INFINITY;
        profileIdxs[igg] = 0;
      }

      scrimp(tSeries, AMean, ASigma, ProfileLength, windowSize, idx,
        profile, profileIdxs, exclusionZone, distances, lastzs);


        DTYPE minDistance = INFINITY;
        int   minDistanceIdx;

        for(int ia = 0; ia < ProfileLength; ia++)
        {
          if(profile[ia] < minDistance)
          {
            minDistance    = profile[ia];
            minDistanceIdx = profileIdxs[ia];
          }

        }

        minDistance = sqrt(minDistance);

        printf("---------------------------------------------------\n");
        printf("Min: %f Idx: %d\n", minDistance, minDistanceIdx);
        printf("---------------------------------------------------\n\n");
        fflush(stdout);

        return 0;
      }
