
#define PATH_TIME_SERIES "./"
#define PATH_RESULTS "./"
#define DTYPE double 
#define ARIT_FACT 8

int loadTimeSeriesFromFile (std::string infilename, std::vector<DTYPE> &A, int &timeSeriesLength);
int saveProfileToFile(std::string outfilename, DTYPE * profile, int * profileIndex, int timeSeriesLength, int windowSize);
