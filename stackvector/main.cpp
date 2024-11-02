#include <iostream>
#include <array>
#include <iterator>
#include <stdexcept>

template <typename T, size_t Capacity>
class StackVector
{
public:
    explicit StackVector(size_t arr_size = 0) : size_(arr_size)
    {
        if (arr_size > Capacity)
        {
            throw std::invalid_argument("StackVector: size is too big");
        }
    }

    T &operator[](size_t index)
    {
        if (index > size_)
        {
            throw std::out_of_range("StackVector: index is out of range");
        }
        return data_[index];
    }

    const T &operator[](size_t index) const
    {
        if (index > size_)
        {
            throw std::out_of_range("StackVector: index is out of range");
        }
        return data_[index];
    }

    auto begin() noexcept { return data_.begin(); }
    auto end() noexcept { return data_.begin() + size_; }
    auto begin() const noexcept { return data_.cbegin(); }
    auto end() const noexcept { return data_.cbegin() + size_; }

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    void push_back(const T &value)
    {
        if (size_ >= capacity_)
        {
            throw std::overflow_error("StackVector: vector is full");
        }
        data_[size_++] = value;
    }

    void pop_back()
    {
        if (size_ == 0)
        {
            throw std::underflow_error("StackVector: vector is empty");
        }
        --size_;
        return data_[size_];
    }

private:
    std::array<T, Capacity> data_;
    size_t size_ = 0;
    size_t capacity_ = Capacity;
};

int main()
{

    StackVector<int, 10> sv;
    for (int i = 0; i < 10; ++i)
    {
        sv.push_back(i);
    }
    for (size_t i = 0; i < sv.size(); ++i)
    {
        std::cout << sv[i] << ' ';
    }
    return 0; // success
}