#pragma once

#include <unordered_map>

#if defined(_WIN32) || defined(_WIN64)
namespace worker {
	using StatusCodeHash = std::hash<worker::StatusCode>;
}
#elif defined(__linux__) || defined(__unix__ )
namespace worker {
	struct StatusCodeHash {
		std::size_t operator()(worker::StatusCode e) const {
			return static_cast<std::size_t>(e);
		}
	};
}
#else
#   error "Unknown compiler"
#endif

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
    std::unordered_map<worker::StatusCode, RetryStrategy, worker::StatusCodeHash> StrategyForStatus;
    int maxRetriesPerCommand;
};

}