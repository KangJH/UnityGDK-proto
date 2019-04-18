#pragma once

#include <memory>
#include <string>

namespace SpatialOS {
namespace RequireExternal {

class ILogger
{
public:
    static std::unique_ptr<ILogger> ApplicationLogger;

    virtual void Debug(const std::string& message) = 0;
    virtual void Info(const std::string& message) = 0;
    virtual void Warn(const std::string& message) = 0;
    virtual void Error(const std::string& message) = 0;
};

}
}