#include <iostream>
#include <vector>
#include <functional>
#include "task.h"

int add(int a, int b)
{
    std::cout << "add " << a + b << std::endl;
    return a + b;
}

double multiply(double a, double b)
{
    std::cout << "multy " << a * b << std::endl;
    return a * b;
}

void func(int i, double d, char ch)
{
    std::cout << "func " << i << " " << d << " " << ch << std::endl;
}

void foo_defer()
{
    std::cout << "defer_func " << std::endl;
}

void checkTask()
{
    std::cout << std::endl
              << "CheckTask" << std::endl;
    char w = 'w';
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;
    double e = 4.1;
    double f = 5.5;

    Task<void, int, double, char> ff(10, func, a, e, w);
    Task<int, int, int> fi(20, add, a, b);
    Task<int, int, int> fi2(20, add, c, d);
    Task<double, double, double> fi3(10, multiply, e, f);
    fi();
    fi2();
}

void checkDispatcherTask()
{
    std::cout << std::endl
              << "checkDispatcherTask" << std::endl;

    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;

    Task<int, int, int> fi(20, add, a, b);
    Task<int, int, int> fi2(20, add, c, d);

    DispatcherTask<int, int, int> dsp;
    dsp.registerFunc(fi);
    dsp.registerFunc(fi2);

    dsp(20);
    a = 4;
    b = 6;
    c = 5;
    d = 8;
    dsp(40);
}

void checkDispatcherAnyTask()
{
    std::cout << std::endl
              << "checkDispatcherAnyTask" << std::endl;
    char w = 'w';
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;
    double e = 4.1;
    double f = 5.5;

    Task<void, int, double, char> ff(10, func, a, e, w);

    Task<int, int, int> fi(20, add, a, b);
    Task<int, int, int> fi2(20, add, c, d);

    Task<double, double, double> fi3(10, multiply, e, f);

    DispatcherAnyTask dsp;
    dsp.registerFunc(fi3);
    dsp.registerFunc(fi);
    dsp.registerFunc(fi2);
    dsp.registerFunc(ff);

    dsp();
}

void checkDeferTask()
{
    std::cout << std::endl
              << "checkDeferTask" << std::endl;
    auto deferedTask = make_deferred(foo_defer);
    std::cout << "any work...." << std::endl;
}

int main()
{
    checkTask();
    checkDispatcherTask();
    checkDispatcherAnyTask();
    checkDeferTask();

    return 0;
}