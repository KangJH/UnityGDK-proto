#include "ExponentialBackoff.h"

#include <cmath>

using namespace SpatialOSSamples;

ExponentialBackoff::ExponentialBackoff(int stepMs) : step(stepMs)
{

}

int ExponentialBackoff::NextWaitMs()
{
    backoffCounter++;
    return static_cast<int>(std::pow(step, backoffCounter));
}

void ExponentialBackoff::Reset()
{
    backoffCounter = 0;
}
