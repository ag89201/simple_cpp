
#include <iostream>
#include <new>
#include <vector>
#include <string>
#include "alloc.h"



heap::pool<4096> Pool;
heap::allocator<heap::default_gurad> Allocator(Pool);

void *operator new(size_t size)
{
    std::cout << "new " << size << std::endl;
    return Allocator.allocate(size);
}

void operator delete(void *ptr) // delete allocated storage
{
    std::cout << "delete 1" << std::endl;
    Allocator.deallocate(ptr);
}

void operator delete(void *ptr, size_t size) // delete allocated storage
{
    std::cout << "delete 2" << size << std::endl;
    Allocator.deallocate(ptr);
}
void operator delete[](void* ptr) // delete allocated storage
{
    std::cout << "delete[]" << std::endl;
    Allocator.deallocate(ptr);
}

std::ostream &operator<<(std::ostream &os, const heap::allocator<heap::default_gurad>::summary::info &info)
{

    return os << "Blocks: " << info.Blocks << std::endl
              << "Size: " << info.Size << std::endl
              << "Max Blocks size: " << info.Block_max_size << std::endl;
}

void TestAlloc()
{
    // Test 1: Allocate
    {
        std::cout << "Before alloc" << std::endl;
        std::cout << "------------" << std::endl;
        std::cout << "Free" << std::endl
                  << Allocator.info().Free << std::endl;
        std::cout << "Used" << std::endl
                  << Allocator.info().Used << std::endl;
        auto p = new int[10]{1,2,3,4,5,6,7,8,9,10};
        auto p2 = new int[10]{-1,-2,-3,-4,-5,-6,-7,-8,-9,-10};
        for(int i=0;i<10;i++)
        {
            std::cout << p[i] << std::endl;
            std::cout << p2[i] << std::endl;
        }

        (*p)++;
        std::cout << "After alloc" << std::endl;
        std::cout << "------------" << std::endl;
        std::cout << "Free" << std::endl
                  << Allocator.info().Free << std::endl;
        std::cout << "Used" << std::endl
                  << Allocator.info().Used << std::endl;

        std::cout << p << std::endl;

        delete[] p;
        p = nullptr;

       std::cout << "After delete alloc" << std::endl;
        std::cout << "------------" << std::endl;
        std::cout << "Free" << std::endl
                  << Allocator.info().Free << std::endl;
        std::cout << "Used" << std::endl
                  << Allocator.info().Used << std::endl;

        std::cout << "------------------" << std::endl;

        delete [] p2;
        p2 = nullptr;

        std::cout << "After delete alloc" << std::endl;
        std::cout << "------------" << std::endl;
        std::cout << "Free" << std::endl
                  << Allocator.info().Free << std::endl;
        std::cout << "Used" << std::endl
                  << Allocator.info().Used << std::endl;

                  
    }
    // Test 2: Bad alloc
    {
        std::cout << "Before alloc" << std::endl;
        std::cout << "------------" << std::endl;
        std::cout << "Free" << std::endl
                  << Allocator.info().Free << std::endl;
        std::cout << "Used" << std::endl
                  << Allocator.info().Used << std::endl;

        auto p = new int[10000];
        if( !p)
        {
            
            std::cout << "Bad alloc" << std::endl;
        }
        
        std::cout << "After alloc" << std::endl;
        std::cout << "------------" << std::endl;
        std::cout << "Free" << std::endl
                  << Allocator.info().Free << std::endl;
        std::cout << "Used" << std::endl
                  << Allocator.info().Used << std::endl;
        delete[] p;

    }
}

// void TestVectorAlloc()
// {
//     {
//         std::cout << "Before alloc" << std::endl;
//         std::cout << "------------" << std::endl;
//         std::cout << "Free" << std::endl
//                   << Allocator.info().Free << std::endl;
//         std::cout << "Used" << std::endl
//                   << Allocator.info().Used << std::endl;

//         // heap::mallocator<int> m(&Allocator);
//         heap::mallocator<double> m(Pool);
//         heap::mallocator<int> m2(Pool);
         
        
//         std::vector<double, heap::mallocator<double>> vec(m);
//         vec.reserve(100);
//         for (int i = 0; i < 10; i++)
//         {
//             vec.push_back(i);
//         }

//         for(int i=0;i<10;i++)
//         {
//             std::cout << vec[i] << std::endl;
//         }

//         std::vector<int, heap::mallocator<int>> vec2(m2);
//         vec2.reserve(100);
//         for (int i = 0; i < 10; i++)
//         {
//             vec2.push_back(i*10);
//         }

//        vec2.erase(vec2.begin(),vec2.end());
//        vec2.clear();
        
        
        
//         std::cout<<"------------------"<<std::endl;
//         std::cout<<vec2.size()<<std::endl;
//         std::cout<<"------------------"<<std::endl;
       
//         for(auto it = vec2.begin();it!=vec2.end();it++)
//         {
//             std::cout << *it << std::endl;
//         }

//         std::cout << "After alloc" << std::endl;
//         std::cout << "------------" << std::endl;
//         std::cout << "Free" << std::endl
//                   << Allocator.info().Free << std::endl;
//         std::cout << "Used" << std::endl
//                   << Allocator.info().Used << std::endl;
//     }

//     std::cout << "After delete alloc" << std::endl;
//     std::cout << "------------" << std::endl;
//     std::cout << "Free" << std::endl
//               << Allocator.info().Free << std::endl;
//     std::cout << "Used" << std::endl
//               << Allocator.info().Used << std::endl;

//     std::cout << "------------------" << std::endl;
// }

void TestAllocString()
{
    {
        std::cout << "Before alloc" << std::endl;
        std::cout << "------------" << std::endl;
        std::cout << "Free" << std::endl
                  << Allocator.info().Free << std::endl;
        std::cout << "Used" << std::endl
                  << Allocator.info().Used << std::endl;


        heap::mallocator<char> m(Pool);
        std::basic_string <char,
                            std::char_traits<char>,
                            heap::mallocator<char> > str(m);
        str.reserve(3000);
        
        for(int i=0;i<300;i++)
        {
            // str[i]='a'+i;
            str+='a'+i;
        }

        str.shrink_to_fit();

        str[0]='J';
        str[1]='e';
            std::cout << str.capacity()<< std::endl;

        std::cout << "After alloc" << std::endl;
        std::cout << "------------" << std::endl;
        std::cout << "Free" << std::endl
                  << Allocator.info().Free << std::endl;    
        std::cout << "Used" << std::endl
                  << Allocator.info().Used << std::endl;

        
    }
    std::cout << "After delete alloc" << std::endl;
    std::cout << "------------" << std::endl;
    std::cout << "Free" << std::endl
              << Allocator.info().Free << std::endl;
    std::cout << "Used" << std::endl
              << Allocator.info().Used << std::endl;

}

int main()
{

    // TestVectorAlloc();
    // TestAlloc();
    TestAllocString();
}