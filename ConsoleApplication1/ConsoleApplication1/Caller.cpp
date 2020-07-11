
#include <iostream>

#include "Caller.h"

namespace Caller
{
    void Executor_1::Execute()
    {
        std::cout << "Executor_1" << std::endl;
    }

    void Executor_2::Execute()
    {
        std::cout << "Executor_2" << std::endl;
    }
}