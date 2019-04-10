#pragma once

#include <unordered_map>


namespace SpatialOS {

enum class RetryStrategy
{
    Never,
    Immediate,
    Backoff
};

struct RetryStrategyConfig
{
public:
    std::unordered_map<worker::StatusCode, RetryStrategy> StrategyForStatus;
    int maxRetriesPerCommand;
};

}