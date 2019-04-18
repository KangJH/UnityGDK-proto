#pragma once

#include <memory>
#include <string>
#include <fstream>

#include "RequireExternal/ILogger.h"

namespace SpatialOSSamples {

/*****************************************************************************************************************/
/* Sample implementation of a logger - user is required to build & inject this dependency into SpatialOS library */
/*****************************************************************************************************************/
class SampleLogger : public SpatialOS::RequireExternal::ILogger
{
public:
    SampleLogger(const std::string& fileName);

    virtual void Debug(const std::string& message) override;

    virtual void Info(const std::string& message) override;
    virtual void Warn(const std::string& message) override;
    virtual void Error(const std::string& message) override;

private:
    std::ofstream outStream;

    void DoLog(const std::string& message);
    std::string TimeStamp();
};

}