#include <iostream>
#include <utility>
#include <array>

enum class ErrorServer : int
{
    Internal_server_err ,
    Not_implemented,
    Bad_gateway,
    Service_unvailable,

    Count
};

template <typename Enum, typename T>
class EnumArray
{
public:
    EnumArray(std::initializer_list<std::pair<Enum, T>> &&values)
    {

        for (const auto &val : values)
        {
            data.at(static_cast<std::size_t>(val.first)) = val.second;            
        }
    }

    // template <typename... Args>
    // explicit constexpr EnumArray(Args &&...values) : data{{std::forward<Args>(values)...}} {}

    constexpr EnumArray() = default;

    constexpr T &operator[](Enum key)
    {
        return data[static_cast<std::size_t>(key)];
    }

    constexpr const T &operator[](Enum key) const
    {
        return data[static_cast<std::size_t>(key)];
    }

    static constexpr std::size_t size() { return N; }

private:
    static constexpr std::size_t N = static_cast<std::size_t>(Enum::Count);

    std::array<T, N> data;
    
};

EnumArray<ErrorServer, const char *> ErrorMsg{
    {ErrorServer::Internal_server_err, "Internal_server_err"},
    {ErrorServer::Not_implemented, "Not_implemented"},
    {ErrorServer::Bad_gateway, "Bad_gateway"},
    {ErrorServer::Service_unvailable, "Service_unvailable"}
};

int main()
{
    std::cout << ErrorMsg.size() << std::endl;
    std::cout << ErrorMsg[ErrorServer::Not_implemented] << std::endl;
    std::cout << ErrorMsg[ErrorServer::Internal_server_err] << std::endl;

    return 0;
}