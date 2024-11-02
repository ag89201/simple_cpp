#include <iostream>
#include <utility>
#include <string_view>
#include <string>
#include <algorithm>
#include <execution>
#include <numeric>
#include <random>
using namespace std::literals;

std::string GeneratorQuery(std::mt19937 &generator, int max_lenght, int space_rate)
{
    const int length = std::uniform_int_distribution<>(0, max_lenght)(generator);

    std::string query(length, ' ');
    for(char & c: query)
    {
        const int rnd=std::uniform_int_distribution<>(0, space_rate-1)(generator);
        if(rnd > 0)
        {
            c='a'+(rnd-1);
        }
    }
    return query;
}

int CountWords(std::string_view str)
{
    if (str.empty())
    {
        return 0;
    }

    return std::transform_reduce(std::execution::par, std::next(str.begin()), str.end(), str.begin(), 0, std::plus<>(), [](char c, char prev_c)
                                 { 
                                    return c != ' ' && prev_c == ' '; 
                                 }) +   (str[0] != ' '); // parallel execution
}
int main()
{
     std::mt19937 generator;
     const std::string s = GeneratorQuery(generator, 100'000'000, 4);
    //  std::cout << s << std::endl;
     std::cout << CountWords(s) << std::endl;

    return 0;
}