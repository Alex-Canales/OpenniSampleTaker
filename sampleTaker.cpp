#include <iostream>
#include <vector>
#include <OpenNI.h>

#include "sampleTaker.h"

using namespace openni;
using namespace std;

SampleTaker::SampleTaker():
    wellSet(false)
{
    Status rc = OpenNI::initialize();
    if(rc != STATUS_OK)
    {
        std::cerr << "Initialize failed" << std::endl;
        std::cerr << OpenNI::getExtendedError() << std::endl;
        wellSet = false;
        return;
    }

    rc = device.open(ANY_DEVICE);
    if(rc != STATUS_OK)
    {
        std::cerr << "Couldn't open device" << std::endl;
        std::cerr << OpenNI::getExtendedError() << std::endl;
        wellSet = false;
        return;
    }

    if(device.getSensorInfo(SENSOR_DEPTH) != NULL)
    {
        rc = depthStream.create(device, SENSOR_DEPTH);
        if (rc != STATUS_OK)
        {
            std::cerr << "Couldn't create depth stream" << std::endl;
            std::cerr << OpenNI::getExtendedError() << std::endl;
            wellSet = false;
            return;
        }
    }

    rc = depthStream.start();
    if (rc != STATUS_OK)
    {
        std::cerr << "Couldn't start the depth stream" << std::endl;
        std::cerr << OpenNI::getExtendedError() << std::endl;
        wellSet = false;
        return;
    }

    wellSet = true;
}

SampleTaker::~SampleTaker()
{
    depthStream.stop();
    depthStream.destroy();
    device.close();
    OpenNI::shutdown();
}

//Controls and getting
bool SampleTaker::takeSample(int numberOfTries)
{
    do
    {
        if(takeSample())
            return true;
        else
            numberOfTries--;
    } while(numberOfTries >= 0);
    return false;
}

bool SampleTaker::takeSample()
{
    VideoFrameRef frame;
    int changedStreamDummy;
    VideoStream* pStream = &depthStream;
    Status rc = OpenNI::waitForAnyStream(&pStream, 1, &changedStreamDummy,
            SAMPLE_READ_WAIT_TIMEOUT);
    if (rc != STATUS_OK)
    {
        std::cerr << "Wait failed! (timeout is " << SAMPLE_READ_WAIT_TIMEOUT;
        std::cerr <<" ms)" << std::endl;
        std::cerr << OpenNI::getExtendedError() << std::endl;
        return false;
    }

    rc = depthStream.readFrame(&frame);
    if(rc != STATUS_OK)
    {
        std::cerr <<"Read failed!" << std::endl;
        std::cerr << OpenNI::getExtendedError() << std::endl;
        return false;
    }

    if(frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_1_MM &&
            frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_100_UM)
    {
        std::cerr <<"Unexpected frame format" << std::endl;
        return false;
    }

    DepthPixel* pDepth = (DepthPixel*)frame.getData();
    int x = 0, y = 0, width = frame.getWidth(), height = frame.getHeight();
    DepthPixel depth;

    vector<Point3> sample;
    Point3 point;
    for(y=0; y < height; y++)
    {
        for(x=0; x < width; x++)
        {
            point.x = x;
            point.y = y;
            point.z = pDepth[y * width + x];

            // I want to take real coordinates:
            // depth = pDepth[y * width + x];
            // rc = CoordinateConverter::convertDepthToWorld(depthStream, x, y,
            //         depth, &(point.x), &(point.y), &(point.z));
            //
            // if(rc != STATUS_OK)
            // {
            //     point.x = 0;
            //     point.y = 0;
            //     point.z = 0;
            // }

            sample.push_back(point);
        }
    }
    samples.push_back(sample);
    return true;
}

vector<Point3> SampleTaker::getAverage()
{
    int iPoint = 0, iSample = 0;
    Point3 point = {0, 0, 0};
    vector<Point3> average;

    if(!hasSample())
    {
        std::cout << "No sample to make an average" << std::endl;
        return average;
    }

    // NOTE: I assume that all samples have the same amount of points (because
    // it is the case for sample from the same device)

    //Initialize average
    for(iPoint=0; iPoint < samples[0].size(); iPoint++)
    {
        average.push_back(point);
    }

    for(iSample=0; iSample < samples.size(); iSample++)
    {
        for(iPoint=0; iPoint < samples[iSample].size(); iPoint++)
        {
            point = samples[iSample][iPoint];
            //risk of overflow here:
            average[iPoint].x += point.x;
            average[iPoint].y += point.y;
            average[iPoint].z += point.z;
        }
    }

    for(iPoint=0; iPoint < average.size(); iPoint++)
    {
        average[iPoint].x /= samples.size();
        average[iPoint].y /= samples.size();
        average[iPoint].z /= samples.size();
    }

    return average;
}

vector<Point3> SampleTaker::getDifferenceBoudaries()
{
    int numberPoints = samples[0].size();
    int indexPoint = 0, indexSample = 0;
    Point3 min = { 0, 0, 0 }, max = { 0, 0, 0 }, point = { 0, 0, 0 };
    vector<Point3> differences;

    //I assume all sizes are the same
    for(indexPoint=0; indexPoint < numberPoints; indexPoint++)
    {
        min = samples[0][indexPoint];
        max = samples[0][indexPoint];

        for(indexSample=0; indexSample < samples.size(); indexSample++)
        {
            point = samples[indexSample][indexPoint];
            if(min.x > point.x)
                min.x = point.x;
            if(min.y > point.y)
                min.y = point.y;
            if(min.z > point.z)
                min.z = point.z;
            if(max.x < point.x)
                max.x = point.x;
            if(max.y < point.y)
                max.y = point.y;
            if(max.z < point.z)
                max.z = point.z;
        }

        point.x = max.x - min.x;
        point.y = max.y - min.y;
        point.z = max.z - min.z;
        differences.push_back(point);
    }

    return differences;
}

void SampleTaker::reset()
{
    samples.clear();
}

//Tests
bool SampleTaker::isWellSet()
{
    return wellSet;
}

bool SampleTaker::hasSample()
{
    return samples.size() > 0;
}

//Printing
void SampleTaker::printSamples()
{
    TEST_SAMPLE_AND_QUIT();
    int indexPoint = 0, indexSample = 0;
    Point3 point;

    for(indexSample=0; indexSample < samples.size(); indexSample++)
    {
        std::cout << "{{ SAMPLE" << indexSample << " }}" << std::endl;
        for(indexPoint=0; indexPoint < samples[indexSample].size(); indexPoint++)
        {
            point = samples[indexSample][indexPoint];
            cout << "(" << point.x << "; " << point.y << "; ";
            cout << point.z << ")" << endl;
        }
        std::cout << "=============" << std::endl;
    }
}

void SampleTaker::printSamplesSideBySide()
{
    TEST_SAMPLE_AND_QUIT()
    int indexPoint = 0, indexSample = 0;
    Point3 point;

    for(indexPoint=0; indexPoint < samples[0].size(); indexPoint++)
    {
        for(indexSample=0; indexSample < samples.size(); indexSample++)
        {
            point = samples[indexSample][indexPoint];
            cout << "(" << point.x << "; " << point.y << "; ";
            cout << point.z << ")  ";
        }

        std::cout << std::endl;
    }
}

void SampleTaker::printDifferences()
{
    TEST_SAMPLE_AND_QUIT()
    vector<Point3> differences = getDifferenceBoudaries();
    printVectorPoints(differences);
}

void SampleTaker::printAverage()
{
    TEST_SAMPLE_AND_QUIT()
    vector<Point3> average = getAverage();
    printVectorPoints(average);
}

void SampleTaker::printVectorPoints(vector<Point3> &v)
{
    for(int i=0; i < v.size(); i++)
    {
        std::cout << "(" << v[i].x << "; " << v[i].y << "; ";
        std::cout << v[i].z << ")" << std::endl;
    }
}

//Unit tests (well, technically not unit tests)
void SampleTaker::testDifferences()
{
    int i = 0, indexPoint = 0, indexSample = 0;
    int debutPoint = samples[0].size() -10;
    Point3 point;

    for(indexPoint=debutPoint; indexPoint < samples[0].size(); indexPoint++)
    {
        for(indexSample=0; indexSample < samples.size(); indexSample++)
        {
            point = samples[indexSample][indexPoint];
            cout << "(" << point.x << "; " << point.y << "; ";
            cout << point.z << ")  ";
        }

        std::cout << std::endl;
    }

    vector<Point3> differences = getDifferenceBoudaries();
    for(i=debutPoint; i < differences.size(); i++)
    {
        std::cout << "(" << differences[i].x << "; " << differences[i].y << "; ";
        std::cout << differences[i].z << ")" << std::endl;
    }
}
