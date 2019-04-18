#pragma once


namespace SpatialOSSamples {

class ExponentialBackoff
{
public:
    ExponentialBackoff(int stepMs);
    int NextWaitMs();
    void Reset();

private:
    int backoffCounter = 0;
    int step = 0;
};

}