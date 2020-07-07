#pragma once

#include "FractalSet.h"

class Julia : public FractalSet
{
public:
    Julia();

private:
    struct JuliaWorker : public FractalSet::Worker
    {
        void Compute() override;
    };
};