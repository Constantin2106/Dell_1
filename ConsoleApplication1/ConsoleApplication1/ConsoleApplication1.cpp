// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <list>

#include "Caller.h"
#include "Function.h"

auto Inc(int i)
{
    std::cout << "Call free function Inc\n";
    return i + 1;
}
auto Add(int a, int b)
{
    std::cout << "Call free function Add\n";
    return a + b;
}

class Class
{
    int m_Val{};

public:
    Class() = default;
    auto Inc(int i)
    {
        std::cout << "Call member function Inc\n";
            
        m_Val = ++i;
        return m_Val;
    }
    auto Add(int a, int b)
    {
        std::cout << "Call member function Add\n";
        return a + b;
    }
};

int main()
{
    std::cout << "Hello World!\n";

    using namespace Utils;

    DG_SUBST(DG_HAS_ARGS_IMPL(__VA_ARGS__, 2, 1, ERROR));
    DG_HAS_ARGS_IMPL(&Inc, 2, 1, ERROR);
    
    DG_BIND_DISAMBIGUATE(DG_SUBST(DG_HAS_ARGS_IMPL(&Inc, 2, 1, ERROR)), &Inc);
    DG_BIND_DISAMBIGUATE(DG_HAS_ARGS(&Inc), &Inc);

    using pIncFun = Delegate<int(int)>;
    std::list<pIncFun> Incs;

    using pAddFun = Delegate<int(int, int)>;
    std::list<pAddFun> Adds;

    Class* cl = new Class();

    Incs.push_back(DG_BIND(&Inc));
    Incs.push_back(DG_BIND(&Class::Inc, cl));
    
    Adds.push_back(DG_BIND(&Class::Add, cl));
    Adds.push_back(DG_BIND(&Add));

    int i = 1, c = 0;
    for (auto f : Incs)
    {
        c += f ? f(i) : 0;
        std::cout << "c = " << c << std::endl;
    }
    c = 0;
    for (auto f : Adds)
    {
        c += f ? f(i, 5) : 0;
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
