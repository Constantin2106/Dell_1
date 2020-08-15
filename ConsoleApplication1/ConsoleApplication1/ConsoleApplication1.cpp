// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <list>

#include "Caller.h"
#include "Function.h"

auto funFree(int i)
{
    std::cout << "Call free function\n";
    return i + 1;
}
class Incrementor
{
    int m_Val{};

public:
    Incrementor() = default;
    auto Calc(int inc)
    {
        std::cout << "Call member function\n";
            
        m_Val += inc;
        return m_Val;
    }
};

int main()
{
    std::cout << "Hello World!\n";

    typedef bc::function<int(int)> func_ptr;
    std::list<func_ptr> funcs;

    Incrementor inc;

    funcs.push_back(BC_BIND(&funFree));
    funcs.push_back(BC_BIND(&Incrementor::Calc, &inc));

    int i = 1, c = 0;
    for (auto f : funcs)
    {
        c += f ? f(i) : 0;
        std::cout << "c = " << c << std::endl;
    }

    /*Caller::Executor_1 ex_1;
    Caller::Executor_2 ex_2;
    ex_1.Call();
    ex_2.Call();
    ex_1.Execute();
    ex_2.Execute();*/

    /*std::list<Caller::Caller*> executors;
    
    executors.push_back(ex_1);
    executors.push_back(ex_2);
    
    for(auto& ex : executors)
        ex*/

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
