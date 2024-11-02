#include <iostream>
#include <array>
#include <iterator>
#include <stdexcept>
#include "../mtest/tests.h"

namespace unit = simple_unit_test;

class ipv4
{
    std::array<unsigned char, 4> data;

public:
    constexpr ipv4() : data{0, 0, 0, 0} {}
    constexpr ipv4(unsigned char const b1, unsigned char const b2, unsigned char const b3, unsigned char const b4) : data{b1, b2, b3, b4} {}

    explicit constexpr ipv4(unsigned long a) : data{
                                                   {static_cast<unsigned char>((a >> 24) & 0xFF),
                                                    static_cast<unsigned char>((a >> 16) & 0xFF),
                                                    static_cast<unsigned char>((a >> 8) & 0xFF),
                                                    static_cast<unsigned char>((a) & 0xFF)}} {}

    ipv4(ipv4 const &other) = default;
    ipv4 &operator=(ipv4 const &other) = default;
    ipv4(ipv4 &&other) = default;
    ipv4 &operator=(ipv4 &&other) = default;

    std::string ToString() const
    {
        return std::to_string(data[0]) + '.' + std::to_string(data[1]) + '.' + std::to_string(data[2]) + '.' + std::to_string(data[3]);
    }

    constexpr unsigned long ToInt() const noexcept
    {
        return (static_cast<unsigned long>(data[0]) << 24) | (static_cast<unsigned long>(data[1]) << 16) | (static_cast<unsigned long>(data[2]) << 8) | static_cast<unsigned long>(data[3]);
    }

    friend std::ostream &operator<<(std::ostream &os, ipv4 const &ip)
    {
        return os << ip.ToString();
    }
    friend std::istream &operator>>(std::istream &is, ipv4 &ip)
    {
        char d1, d2, d3;
        int b1, b2, b3, b4;
        is >> b1 >> d1 >> b2 >> d2 >> b3 >> d3 >> b4;
        if (d1 == '.' && d2 == '.' && d3 == '.')
            ip = ipv4(b1, b2, b3, b4);
        else
            is.setstate(std::ios::failbit);
        return is;
    }

    ipv4& operator++()
    {
        *this = ipv4(1+ToInt());
        return *this;
    }

    ipv4 operator++(int)
    {
        ipv4 temp = *this;
        ++*this;
        return temp;
    }

    friend bool operator==(ipv4 const &lhs, ipv4 const &rhs) noexcept
    {
        return lhs.ToInt() == rhs.ToInt();
    }

    friend bool operator!=(ipv4 const &lhs, ipv4 const &rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend bool operator<(ipv4 const &lhs, ipv4 const &rhs) noexcept
    {
        return lhs.ToInt() < rhs.ToInt();
    }

    friend bool operator>(ipv4 const &lhs, ipv4 const &rhs) noexcept
    {
        return rhs<lhs;
    }

    friend bool operator<=(ipv4 const &lhs, ipv4 const &rhs) noexcept
    {
        return !(rhs<lhs);
    }
    friend bool operator>=(ipv4 const &lhs, ipv4 const &rhs) noexcept
    {
        return !(lhs<rhs);
    }
};



void TestIpToString()
{
    // Test code here
    ipv4 ip(192, 168, 1, 10);
    unit::ASSERT(ip.ToString() == "192.168.1.10");
}

void TestOstreamOperator()
{
    ipv4 ip(192, 168, 1, 10);
    std::stringstream ss;
    ss << ip;
    unit::ASSERT(ss.str() == "192.168.1.10");
}

void TestIstreamOperator()
{
    ipv4 ip;
    std::stringstream ss("192.168.1.10");
    ss >> ip;
    unit::ASSERT(ip.ToString() == "192.168.1.10");

    std::stringstream ss2("192.168.1");
    ss2 >> ip;
    unit::ASSERT(ip.ToString() != "192.168.1.0");

    std::stringstream ss3("192.168.1.256");
    ss3 >> ip;
    unit::ASSERT(ip.ToString() != "192.168.1.256");

    std::stringstream ss4("192.168.1.256.256");
    ss4 >> ip;
    unit::ASSERT(ip.ToString() != "192.168.1.256.256");

    std::stringstream ss5("192.168.1.256a");
    ss5 >> ip;
    unit::ASSERT(ip.ToString() != "192.168.1.256a");
    
    std::stringstream ss6("192.168.1.256 256");
    ss6 >> ip;
    unit::ASSERT(ip.ToString() != "192.168.1.256 256");
    
    std::stringstream ss7("192.168.1.256\n256");
    ss7 >> ip;
    unit::ASSERT(ip.ToString() != "192.168.1.256\n256");

    std::stringstream ss8("192.168.1x256");
    ss8 >> ip;
    unit::ASSERT(ss8.fail());


    std::stringstream ss9("192.168.257.256 ");
    ss9 >> ip;
    std::cout << ip.ToString() << std::endl;

}

void TestIpToInt()
{
    ipv4 ip(192, 168, 1, 10);
    unit::ASSERT(ip.ToInt() == 0xC0A8010A);
}

void TestOperatorplusplus()
{
    ipv4 ip(192, 168, 1, 10);
    unit::ASSERT(++ip == ipv4(192, 168, 1, 11));
    unit::ASSERT(ip++ == ipv4(192, 168, 1, 11));
    unit::ASSERT(ip == ipv4(192, 168, 1, 12));
    ipv4 ip2(192, 168, 1, 255);
    unit::ASSERT(++ip2 == ipv4(192, 168, 2, 0));
    unit::ASSERT(ip2++ == ipv4(192, 168, 2, 0));
    unit::ASSERT(ip2 == ipv4(192, 168, 2, 1));
}

void TestOperatorEqual()
{
    ipv4 ip(192, 168, 1, 10);
    unit::ASSERT(ip > ipv4(192, 168, 1, 9));
    unit::ASSERT(ip >= ipv4(192, 168, 1, 10));
    unit::ASSERT(ip < ipv4(192, 168, 1, 11));
    unit::ASSERT(ip <= ipv4(192, 168, 1, 10));
}

void TestRangeIp()
{
    ipv4 ip(192, 168, 1, 10);
    ipv4 ip2(192, 168, 1, 20);
    for(ipv4 i = ip; i < ip2; ++i)
    {
        unit::ASSERT(i < ip2);
        unit::ASSERT(i >= ip);
        std::cout<<i<<std::endl;
        
    }

}

int main()
{
    unit::TestRunner runner;
    runner.RunTest(TestIpToString, "TestIpToString");
    runner.RunTest(TestOstreamOperator, "TestOstreamOperator");
    runner.RunTest(TestIpToInt, "TestIpToInt");
    runner.RunTest(TestIstreamOperator, "TestIstreamOperator");
    runner.RunTest(TestOperatorplusplus, "TestOperatorplusplus");
    runner.RunTest(TestOperatorEqual, "TestOperatorEqual");
    runner.RunTest(TestRangeIp, "TestRangeIp");
    return 0;
}