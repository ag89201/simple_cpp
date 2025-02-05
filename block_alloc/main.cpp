
#include <iostream>
#include <new>
#include <vector>
#include <string>
#include "../testfunc/log_duration.h"
#include "alloc.h"

heap::pool<4096> Pool;
heap::allocator<heap::default_guard> Allocator(Pool);

// void *operator new(size_t size)
// {
//     std::cout << "new " << size << std::endl;
//     return Allocator.allocate(size);
// }

// void operator delete(void *ptr) // delete allocated storage
// {
//     std::cout << "delete 1" << std::endl;
//     Allocator.deallocate(ptr);
// }

// void operator delete(void *ptr, size_t size) // delete allocated storage
// {
//     std::cout << "delete 2: " << size << std::endl;
//     Allocator.deallocate(ptr);
// }
// void operator delete[](void *ptr) // delete allocated storage
// {
//     std::cout << "delete[]" << std::endl;
//     Allocator.deallocate(ptr);
// }

std::ostream &operator<<(std::ostream &os, const heap::allocator<heap::default_guard>::Summary::Info &info)
{

    return os << "Blocks: " << info.blocks << std::endl
              << "Size: " << info.size << std::endl
              << "Max Blocks size: " << info.block_max_size << std::endl;
}

void print_info(const char *title)
{
    std::cout << title << std::endl;
    std::cout << "------------" << std::endl;
    std::cout << "Free" << std::endl
              << Allocator.info().free << std::endl;
    std::cout << "Used" << std::endl
              << Allocator.info().used << std::endl;
}

void print_info(const char *title, const heap::allocator<heap::default_guard>::Summary &info)
{
    std::cout << title << std::endl;
    std::cout << "------------" << std::endl;
    std::cout << "Free" << std::endl
              << info.free << std::endl;
    std::cout << "Used" << std::endl
              << info.used << std::endl;
}

void TestAlloc()
{
    std::cout << "Test 1: Allocate" << std::endl;

    {

        print_info("Before alloc");
        auto p = new int[10]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        auto p2 = new int[10]{-1, -2, -3, -4, -5, -6, -7, -8, -9, -10};
        for (int i = 0; i < 10; i++)
        {
            std::cout << p[i] << std::endl;
            std::cout << p2[i] << std::endl;
        }

        // (*p)++;

        print_info("After alloc");

        delete[] p;
        p = nullptr;

        print_info("After delete alloc");

        delete[] p2;
        p2 = nullptr;

        print_info("After delete alloc");
    }
    std::cout << "Test 2: Bad alloc" << std::endl;
    {

        print_info("Before alloc");

        auto p = new int[1000];
        if (!p)
        {

            std::cout << "Bad alloc" << std::endl;
        }

        std::cout << "After alloc" << std::endl;
        print_info("After alloc");
        delete[] p;
    }
}

void TestVectorAlloc()
{
    {
        heap::pool<4096> _pool_;
        heap::mallocator<double> m(_pool_);
        print_info("begin heap", m.info());
        {
            std::vector<double, heap::mallocator<double>> vec(m);
            vec.reserve(100);
            print_info("reserverd heap", m.info());
            for (int i = 0; i < 10; i++)
            {
                vec.push_back(i);
            }

            for (int i = 0; i < 10; i++)
            {
                std::cout << vec[i] << std::endl;
            }
        }
        print_info("end heap", m.info());
    }
}

void TestAllocString()
{
    {
        std::cout << "Before alloc" << std::endl;
        std::cout << "------------" << std::endl;
        std::cout << "Free" << std::endl
                  << Allocator.info().free << std::endl;
        std::cout << "Used" << std::endl
                  << Allocator.info().used << std::endl;

        heap::mallocator<char> m(Pool);
        std::basic_string<char,
                          std::char_traits<char>,
                          heap::mallocator<char>>
            str(m);
        str.reserve(3000);

        for (int i = 0; i < 100; i++)
        {
            // str[i]='a'+i;
            str += 'a' + i;
        }
        std::cout << "string capacity: " << str.capacity() << std::endl;
        std::cout << "shrink_to_fit" << std::endl;
        // str.shrink_to_fit();

        str[0] = 'J';
        str[1] = 'e';
        std::cout << "string capacity: " << str.capacity() << std::endl;

        std::cout << "After alloc" << std::endl;
        std::cout << "------------" << std::endl;
        std::cout << "Free" << std::endl
                  << Allocator.info().free << std::endl;
        std::cout << "Used" << std::endl
                  << Allocator.info().used << std::endl;
    }
    std::cout << "After delete alloc" << std::endl;
    std::cout << "------------" << std::endl;
    std::cout << "Free" << std::endl
              << Allocator.info().free << std::endl;
    std::cout << "Used" << std::endl
              << Allocator.info().used << std::endl;
}

void TestSpeedAllocArray()
{
    print_info("begin heap");

    print_info("end heap");
}

void TestSpeedAlloc()
{

    {

        heap::pool<4096000> _pool_;
        heap::mallocator<uint8_t> m(_pool_);
        print_info("start heap", m.info());
        std::cout<<"max size "<< m.max_size()<<std::endl;
        {
            LOG_DURATION("my allocator");
            std::vector<uint8_t, heap::mallocator<uint8_t>> vec(m);

            vec.reserve(4095952);
            std::cout<<"max size "<< m.max_size()<<std::endl;
            print_info("reserv", m.info());
            for (int i = 0; i < 4095952; i++)
            {
                vec.push_back(i);
                // vec[i]=i;
            }
            // std::cout<<"vec_size "<<vec.size()<<std::endl;
            // std::cout<<"sizeof(vec)"<<vec.size()*sizeof(vec[0])<<std::endl;
            // print_info("push", m.info());

            std::cout<<"max size "<< m.max_size()<<std::endl;
        }
        std::cout<<"max size "<< m.max_size()<<std::endl;
        // print_info("end heap", m.info());

        {
            LOG_DURATION("std allocator");
            std::vector<uint8_t> vec;
           // vec.reserve(1000000);
            for (int i = 0; i < 4095952; i++)
            {
                vec.push_back(i);
            }
        }
    }
}

int main()
{

    // TestAlloc();
    // TestVectorAlloc();
    // TestAllocString();
    TestSpeedAlloc();
    // TestSpeedAllocArray();
}