#include <iostream>
#include <array>
#include <iterator>
#include <stdexcept>
#include <vector>
#include <list>
#include <math.h>
#include <numbers>
#include "../mtest/tests.h"

// constexpr double M_PI = 3.14159265358979323846 ;

namespace unit = simple_unit_test;

// выбор минимального значения с переменным числом аргументов

template <typename T>
T minimum(T const a, T const b)
{
    return a < b ? a : b;
}

template <typename T, typename... T1>
T minimum(T const a, T1 const &...args)
{
    return minimum(a, minimum(args...));
}

template <class Compare, typename T>
T minimumc(Compare comp, T const a, T const b)
{
    return comp(a, b) ? a : b;
}

template <class Compare, typename T, typename... T1>
T minimumc(Compare comp, T const a, T1 const &...args)
{
    return minimumc(comp, a, minimumc(comp, args...));
}

void TestMinimumFunc()
{
    unit::ASSERT_HINT(minimum(1, 2) == 1, "minimum(1, 2) == 1");
    unit::ASSERT_HINT(minimum(2, 1) == 1, "minimum(2, 1) == 1");
    unit::ASSERT_HINT(minimum(1, 2, 3) == 1, "minimum(1, 2, 3) == 1");
    unit::ASSERT_HINT(minimum(2, 3, 1) == 1, "minimum(2, 3, 1) == 1");
    unit::ASSERT_HINT(minimum(3, 2, 1) == 1, "minimum(3, 2, 1) == 1");
    unit::ASSERT_HINT(minimum(1, 1, 2) == 1, "minimum(1, 1, 2) == 1");
}

//------------------------------------------------------------------------------
// Проверка наличия в контейнере любого, всех и ни одного из указанных значений

template <class C, class T>
bool contains(C const &c, T const &val)
{
    return std::find(c.begin(), c.end(), val) != c.end();
}

template <class C, class... T>
bool contains_any(C const &c, T &&...args)
{
    return (... || contains(c, args));
}

template <class C, class... T>
bool contains_all(C const &c, T &&...args)
{
    return (... && contains(c, args));
}

template <class C, class... T>
bool contains_none(C const &c, T &&...args)
{
    return !contains_any(c, args...);
}

void TestContainsFunc()
{
    std::vector<int> v{1, 2, 3, 4, 5};
    std::array<int, 5> a{1, 2, 3, 4, 5};
    unit::ASSERT_HINT(contains(v, 3), "contains(v, 3)");
    unit::ASSERT_HINT(contains(a, 3), "contains(a, 3)");
    unit::ASSERT_HINT(contains_any(v, 3, 6, 7), "contains_any(v, 3, 6, 7)");
    unit::ASSERT_HINT(contains_all(v, 1, 2, 3, 4, 5), "contains_all(v, 1, 2, 3, 4, 5)");
    unit::ASSERT_HINT(contains_none(v, 6, 7, 8), "contains_none(v, 6, 7, 8)");
}

// добавление диапазона значений в контейнер
template <typename C, typename... Args>
void push_back(C &c, Args &&...args)
{
    (c.push_back(args), ...);
}

void TestPushbackFunc()
{
    std::vector<int> v;
    push_back(v, 1, 2, 3, 4, 5);
    unit::ASSERT_EQUAL_HINT(v.size(), 5u, "push_back(v, 1, 2, 3, 4, 5)");
    unit::ASSERT_EQUAL_HINT(v[0], 1, "push_back(v, 1, 2, 3, 4, 5)");
    unit::ASSERT_EQUAL_HINT(v[4], 5, "push_back(v, 1, 2, 3, 4, 5)");
    // std::array<int, 5> a;
    // push_back(a, 1, 2, 3, 4, 5);
    // unit::ASSERT_EQUAL_HINT(a.size(), 5u, "push_back(a, 1, 2, 3, 4, 5)");
    // unit::ASSERT_EQUAL_HINT(a[0], 1, "push_back(a, 1, 2, 3, 4, 5)");
    // unit::ASSERT_EQUAL_HINT(a[4], 5, "push_back(a, 1, 2, 3, 4, 5)");
    std::list<int> l;
    push_back(l, 1, 2, 3, 4, 5);
    unit::ASSERT_EQUAL_HINT(l.size(), 5u, "push_back(l, 1, 2, 3, 4, 5)");
    unit::ASSERT_EQUAL_HINT(l.front(), 1, "push_back(l, 1, 2, 3, 4, 5)");
    unit::ASSERT_EQUAL_HINT(l.back(), 5, "push_back(l, 1, 2, 3, 4, 5)");
}

// литералы
bool are_equal(double const d1, double const d2, double const eps = 0.0001)
{
    return std::fabs(d1 - d2) < eps;
}

namespace temperature
{
    enum class scale
    {
        celsius,
        fahrenheit,
        kelvin,
        deg,
        rad
    };
    template <scale S>
    class quantity
    {
        const double amount;

    public:
        constexpr explicit quantity(double const a) : amount(a) {}
        explicit operator double() const { return amount; }
    };

    template <scale S>
    inline bool operator==(quantity<S> const &lhs, quantity<S> const &rhs)
    {
        return are_equal(static_cast<double>(lhs), static_cast<double>(rhs));
    }

    template <scale S>
    inline bool operator!=(quantity<S> const &lhs, quantity<S> const &rhs)
    {
        return !(lhs == rhs);
    }

    template <scale S>
    inline bool operator<(quantity<S> const &lhs, quantity<S> const &rhs)
    {
        return static_cast<double>(lhs) < static_cast<double>(rhs);
    }

    template <scale S>
    inline bool operator>(quantity<S> const &lhs, quantity<S> const &rhs)
    {
        return static_cast<double>(lhs) > static_cast<double>(rhs);
    }

    template <scale S>
    inline bool operator<=(quantity<S> const &lhs, quantity<S> const &rhs)
    {
        return !(lhs > rhs);
    }

    template <scale S>
    inline bool operator>=(quantity<S> const &lhs, quantity<S> const &rhs)
    {
        return !(lhs < rhs);
    }

    template <scale S, scale T>
    inline quantity<T> operator+(quantity<S> const &lhs, quantity<S> const &rhs)
    {
        return quantity<T>(static_cast<double>(lhs) + static_cast<double>(rhs));
    }

    template <scale S, scale T>
    inline quantity<T> operator-(quantity<S> const &lhs, quantity<S> const &rhs)
    {
        return quantity<T>(static_cast<double>(lhs) - static_cast<double>(rhs));
    }

    template <scale S, scale T>
    inline quantity<T> operator*(quantity<S> const &lhs, double const rhs)
    {
        return quantity<T>(static_cast<double>(lhs) * rhs);
    }

    template <scale S, scale T>
    inline quantity<T> operator*(double const lhs, quantity<S> const &rhs)
    {
        return quantity<T>(lhs * static_cast<double>(rhs));
    }

    template <scale S, scale T>
    inline quantity<T> operator/(quantity<S> const &lhs, double const rhs)
    {
        return quantity<T>(static_cast<double>(lhs) / rhs);
    }

    template <scale S, scale T>
    inline quantity<T> operator/(double const lhs, quantity<S> const &rhs)
    {
        return quantity<T>(lhs / static_cast<double>(rhs));
    }

    // convert

    template <scale S, scale R>
    struct conversion_traits
    {
        static double convert(double const value) = delete;
        
    };

    template <>
    struct conversion_traits<scale::celsius, scale::kelvin>
    {
        static double convert(double const value) { return value + 273.15; }
    };

    template <>
    struct conversion_traits<scale::kelvin, scale::celsius>
    {
        static double convert(double const value) { return value - 273.15; }
    };

    template <>
    struct conversion_traits<scale::celsius, scale::fahrenheit>
    {
        static double convert(double const value) { return value * 9.0 / 5.0 + 32; }
    };

    template <>
    struct conversion_traits<scale::fahrenheit, scale::celsius>
    {
        static double convert(double const value) { return (value - 32) * 5.0 / 9.0; }
    };

    template <>
    struct conversion_traits<scale::fahrenheit, scale::kelvin>
    {
        static double convert(double const value) { return (value + 459.67) * 5.0 / 9.0; }
    };

    template <>
    struct conversion_traits<scale::kelvin, scale::fahrenheit>
    {
        static double convert(double const value) { return value * 9.0 / 5.0 - 459.67; }
    };

    template <scale R, scale S>
    constexpr quantity<R> temperature_cast(quantity<S> const value)
    {
        return quantity<R>(conversion_traits<S, R>::convert(static_cast<double>(value)));
    }

    template<>
    struct conversion_traits<scale::deg, scale::rad>
    {
        static double convert(double const value) { return value * M_PI / 180.0; }
    };

    template<>
    struct conversion_traits<scale::rad, scale::deg>
    {
        static double convert(double const value) { return value * 180.0 / M_PI; }
    };

    namespace literals
    {

        constexpr quantity<scale::celsius> operator"" _C(long double const amount)
        {
            return quantity<scale::celsius>(amount);
        }
        constexpr quantity<scale::fahrenheit> operator"" _F(long double const amount)
        {
            return quantity<scale::fahrenheit>(amount);
        }
        constexpr quantity<scale::kelvin> operator"" _K(long double const amount)
        {
            return quantity<scale::kelvin>(amount);
        }

        constexpr quantity<scale::deg> operator"" _deg(long double const amount)
        {
            return quantity<scale::deg>(amount);
        }

        constexpr quantity<scale::rad> operator"" _rad(long double const amount)
        {
            return quantity<scale::rad>(amount);
        }
    }
}

void TestTemperatureFunc()
{
    using namespace temperature;
    using namespace temperature::literals;
    auto t1{36.5_C};
    auto t2{97.7_F};
    auto t3{309.65_K};

    auto t4{359._deg};
    auto t5{6.265732_rad};

    auto tf = temperature_cast<scale::fahrenheit>(t1);
    auto tc = temperature_cast<scale::celsius>(t2);
    auto tk = temperature_cast<scale::kelvin>(t1);
    

    unit::ASSERT(t1 == tc);
    unit::ASSERT(t2 == tf);
    unit::ASSERT(t3 == tk);


    unit::ASSERT(tc == 36.5_C);
    unit::ASSERT(tf == 97.7_F);
    unit::ASSERT(tk == 309.65_K);

    unit::ASSERT(t4 == 359._deg);
    unit::ASSERT(t5 == 6.265732_rad);
    unit::ASSERT(t4 == temperature_cast<scale::deg>(t5));
    unit::ASSERT(t5 == temperature_cast<scale::rad>(t4));
    unit::ASSERT(t4 == temperature_cast<scale::deg>(6.265732_rad));
    unit::ASSERT(t5 == temperature_cast<scale::rad>(359._deg));

    // std::cout<<temperature_cast<scale::deg>(359._deg)<<std::endl;
    // std::cout<<temperature_cast<scale::deg>(6.265732_rad)<<std::endl
    std::cout<<static_cast<double>(t4)<<std::endl;
    std::cout<<static_cast<double>(temperature_cast<scale::deg>(6.265732_rad))<<std::endl;
    std::cout<<static_cast<double>(t1)<<std::endl;
    std::cout<<static_cast<double>(t2)<<std::endl;  
    std::cout<<static_cast<double>(t3)<<std::endl;
}

int main()
{

    unit::TestRunner tr;
    tr.RunTest(TestMinimumFunc, "TestMinimumFunc");
    tr.RunTest(TestContainsFunc, "TestContainsFunc");
    tr.RunTest(TestPushbackFunc, "TestPushbackFunc");
    tr.RunTest(TestTemperatureFunc, "TestTemperatureFunc");

    return 0;
}