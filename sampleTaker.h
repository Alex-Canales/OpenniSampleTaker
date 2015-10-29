#ifndef _SAMPLETAKER_H_
#define _SAMPLETAKER_H_

#include <vector>

#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms

#define TEST_SAMPLE_AND_QUIT() if(!hasSample()) \
    { \
        std::cout << "No sample." << std::endl; \
        return; \
    } \

using namespace openni;
using namespace std;

typedef struct {
    float x, y, z;
} Point3;

class SampleTaker
{
public:
    vector<vector<Point3> > samples;

    SampleTaker();
    ~SampleTaker();

    //Controls and getting
    bool takeSample();
    bool takeSample(int numberOfTries);
    vector<Point3> getAverage();
    vector<Point3> getDifferenceBoudaries();  //TODO: better name?
    void reset();

    //Tests
    bool isWellSet();
    bool hasSample();

    //Printing
    void printSamples();
    void printSamplesSideBySide();
    void printDifferences();
    void printAverage();

    //Unit tests (well, technically not unit tests)
    void testDifferences();
private:
    Device device;
    VideoStream depthStream;
    bool wellSet;

    void printVectorPoints(vector<Point3> &v);
};

#endif
