#include "SampleLogger.h"

#include <iostream>
#include <ctime>

using namespace SpatialOSSamples;

SampleLogger::SampleLogger(const std::string& fileName, worker::Connection& inConnection) 
	: outStream(fileName), connection(inConnection)
{

}

void SampleLogger::Debug(const std::string& message)
{
    DoLog(worker::LogLevel::kDebug, "[DEBUG] " + message);
}

void SampleLogger::Info(const std::string& message)
{
    DoLog(worker::LogLevel::kInfo, "[INFO] " + message);
}

void SampleLogger::Warn(const std::string& message)
{
    DoLog(worker::LogLevel::kWarn, "[WARN] " + message);
}

void SampleLogger::Error(const std::string& message)
{
	DoLog(worker::LogLevel::kError, "[ERROR] " + message);
}


void SampleLogger::DoLog(worker::LogLevel level, const std::string& message)
{
    std::cerr << "<" << TimeStamp() << "> " << message << std::endl;
	outStream << "<" << TimeStamp() << "> " << message << std::endl;
	const std::string loggername = "AIWorker";
	connection.SendLogMessage(level, loggername, message);
	
}

std::string SampleLogger::TimeStamp()
{
    time_t time = std::time(nullptr);
    std::string timeStamp = std::ctime(&time);
    auto endlPos = timeStamp.find_last_of("\n");
    return timeStamp.erase(endlPos, timeStamp.size() - endlPos);
}
