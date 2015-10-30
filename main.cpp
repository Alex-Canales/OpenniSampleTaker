#include <iostream>
#include <unistd.h>
#include <string>
#include <fstream>
#include <OpenNI.h>

#include "sampleTaker.h"
#include "displayer.h"

bool savePointsToFiles(vector<Point3> points, std::string fileName)
{
    std::ofstream file;

    file.open(fileName.c_str());
    if(!file.is_open())
        return false;

    for(size_t i=0; i < points.size(); i++)
    {
        file << points[i].x << " " << points[i].y << " " << points[i].z << "\n";
    }

    file.close();
    return true;
}

void displaySample(Displayer displayer, vector<Point3> sample)
{
    int i = 0, grayscale = 0, division = 1000;
    float value = 0;
    Point3 point;

    for(i=0; i < sample.size(); i++)
    {
        point = sample[i];
        value = static_cast<float>(static_cast<int>(point.z) % division);
        value = value / static_cast<float>(division) * 255.0;
        grayscale = static_cast<int>(value);
        displayer.setColor(grayscale, grayscale, grayscale, point.x, point.y);
    }

    displayer.refresh();
}

void printInstructions()
{
    std::cout << "This software takes samples with the sensor." << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "\ts - Take a sample" << std::endl;
    std::cout << "\ta - Show average of the taken samples" << std::endl;
    std::cout << "\tf - Save average in file average.xyz" << std::endl;
    std::cout << "\tr - Reset data" << std::endl;
    std::cout << "\th - Print instructions" << std::endl;
    std::cout << "\tEsc - Quit" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[])
{
    int i(0);
    bool toContinue(true);
    vector<Point3> average;
    SampleTaker taker;
    if(!taker.isWellSet())
    {
        std::cout << "Sample taker not well initialized." << std::endl;
        return 1;
    }

    Displayer displayer;
    //TODO: set the good width and height
    if(!displayer.initialize(600, 600, "Sample taker"))
    {
        std::cout << "Displayer not well initialized." << std::endl;
        return 1;
    }

    printInstructions();

    SDL_Event event;

    while(toContinue)
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    toContinue = false;
                    break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_ESCAPE)
                        toContinue = false;
                    else if(event.key.keysym.sym == SDLK_h)
                        printInstructions();
                    else if(event.key.keysym.sym == SDLK_r)
                    {
                        taker.reset();

                        //TODO: have a way to reset screen

                        std::cout << "Reset" << std::endl;
                    }
                    else if(event.key.keysym.sym == SDLK_s)
                    {
                        if(taker.takeSample(3))
                        {
                            displaySample(displayer,
                                    taker.samples[taker.samples.size() - 1]);
                            std::cout << "Sample taken and displayed" << std::endl;
                        }
                        else
                            std::cout << "No sample was taken!" << std::endl;
                    }
                    else if(event.key.keysym.sym == SDLK_a)
                    {
                        average = taker.getAverage();
                        if(average.size() > 0)
                        {
                            displaySample(displayer, average);
                            std::cout << "Average displayed" << std::endl;
                        }
                        else
                            std::cout << "No sample was taken!" << std::endl;
                    }
                    else if(event.key.keysym.sym == SDLK_f)
                    {
                        average = taker.getAverage();
                        if(average.size() > 0)
                        {
                            savePointsToFiles(average, "average.xyz");
                            std::cout << "Average saved" << std::endl;
                        }
                        else
                            std::cout << "No sample was taken!" << std::endl;
                    }
                    break;
            }
        }
    }

    //TODO: clear memory!
    displayer.destroy();

    return 0;
}
