#include "SampleLogger.h"

#include <iostream>
#include <ctime>

using namespace SpatialOSSamples;

SampleLogger::SampleLogger(const std::string& fileName) : outStream(fileName)
{

}

void SampleLogger::Debug(const std::string& message)
{
    DoLog("[DEBUG] " + message);
}

void SampleLogger::Info(const std::string& message)
{
    DoLog("[INFO] " + message);
}

void SampleLogger::Warn(const std::string& message)
{
    DoLog("[WARN] " + message);
}

void SampleLogger::Error(const std::string& message)
{
    DoLog("[ERROR] " + message);
}


void SampleLogger::DoLog(const std::string& message)
{
    std::cerr << "<" << TimeStamp() << "> " << message << std::endl;
    outStream << "<" << TimeStamp() << "> " << message << std::endl;
}

std::string SampleLogger::TimeStamp()
{
    time_t time = std::time(nullptr);
    std::string timeStamp = std::ctime(&time);
    auto endlPos = timeStamp.find_last_of("\n");
    return timeStamp.erase(endlPos, timeStamp.size() - endlPos);
}
