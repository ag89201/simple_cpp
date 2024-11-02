#include <iostream>
#include <array>
#include <iterator>
#include <stdexcept>
#include <vector>
#include "../mtest/tests.h"

namespace unit = simple_unit_test;

template <class T, size_t R, size_t C>
class array2d
{
    using value_type = T;
    using iterator = value_type *;
    using const_iterator = const value_type *;
    using reference = value_type &;
    using const_reference = const value_type &;
    std::vector<value_type> m_data;
    static constexpr size_t m_rows = R;
    static constexpr size_t m_cols = C;

public:
    array2d() : m_data(R * C) {}

    explicit array2d(std::initializer_list<T> init) : m_data{init} {}

    constexpr T *data() noexcept { return m_data.data(); }

    constexpr T const *data() const noexcept { return m_data.data(); }

    constexpr T &at(size_t const r, size_t const c)
    {
        return m_data.at(r * m_cols + c);
    }

    constexpr T const &at(size_t const r, size_t const c) const
    {
        return m_data.at(r * m_cols + c);
    }

    constexpr T &operator()(size_t const r, size_t const c)
    {
        return m_data[r * m_cols + c];
    }

    constexpr T const &operator()(size_t const r, size_t const c) const
    {
        return m_data[r * m_cols + c];
    }

    constexpr size_t rows() const noexcept { return m_rows; }
    constexpr size_t cols() const noexcept { return m_cols; }

    constexpr bool empty() const noexcept { return m_data.empty(); }
    constexpr size_t size() const noexcept { return m_data.size(); }

    constexpr reference operator[](size_t const i) { return m_data[i]; }
    constexpr const_reference operator[](size_t const i) const { return m_data[i]; }

    void fill(value_type const &value)
    {
        std::fill(std::begin(m_data), std::end(m_data), value);
    }

    void swap(array2d &other) noexcept
    {
        m_data.swap(other.m_data);
    }

    const_iterator cbegin() const { return m_data.data(); }
    const_iterator cend() const { return m_data.data() + m_data.size(); }
    iterator begin() { return m_data.data(); }
    iterator end() { return m_data.data() + m_data.size(); }
};

template <typename T, size_t r, size_t c>
std::ostream &operator<<(std::ostream &os, array2d<T, r, c> const &a)
{
    for (size_t i = 0; i < r; ++i)
    {
        for (size_t j = 0; j < c; ++j)
            os << a(i, j) << ' ';
        os << std::endl;
    }
    return os;
}

void TestArr2d()
{
    array2d<int, 1, 3> a2{3, 4, 1};
    std::cout << a2 << std::endl;

    array2d<int, 1, 3> a;
    for (int i = 0; i < a.rows(); ++i)
        for (int j = 0; j < a.cols(); ++j)
            a(i, j) = i * a.cols() + j;
    std::cout << a << std::endl;
    std::cout << std::endl;

    array2d<int, 1, 3> b(a);
    std::cout << b << std::endl;
    std::cout << std::endl;

    b = a2;
    std::cout << b << std::endl;
    std::cout << std::endl;

    array2d<int, 1, 3> c;
    c.fill(777);
    std::cout << c << std::endl;
    std::cout << std::boolalpha << c.empty() << std::endl;
    std::cout << std::endl;

    array2d<int, 1, 3> d(std::move(c));
    std::cout << d << std::endl;
    std::cout << std::endl;

    std::cout << std::boolalpha << c.empty() << std::endl;
}

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

int main()
{
    TestArr2d();

    std::cout << "minimum(1, 2, 3, 4, 5) = " << minimum(1, 2, 3, 4, 5) << std::endl;
    std::cout << "minimum(1,2) = " << minimum(1, 2) << std::endl;
    std::cout << "(minimum(std::less<>(),1,2,3,0) = " << (minimumc(std::less<>(), 1, 2,3 ,0)) << std::endl;
    

    return 0;
}