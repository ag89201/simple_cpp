#pragma once
#include <functional>
#include <vector>
#include <type_traits>
#include <iostream>
#include <memory>

template <typename F, typename Tuple, bool Enough, int TotalArgs, int... N>
struct call_impl
{
    auto static call(F f, Tuple &&t)
    {
        return call_impl<F, Tuple, TotalArgs == 1 + sizeof...(N),
                         TotalArgs, N..., sizeof...(N)>::call(f, std::forward<Tuple>(t));
    }
};

template <typename F, typename Tuple, int TotalArgs, int... N>
struct call_impl<F, Tuple, true, TotalArgs, N...>
{
    auto static call(F f, Tuple &&t)
    {
        return f(std::get<N>(std::forward<Tuple &>(t))...);
    }
};

template <typename F, typename Tuple>
auto call(F f, Tuple &&t)
{
    typedef typename std::decay<Tuple>::type type;
    return call_impl<F, Tuple, 0 == std::tuple_size<type>::value,
                     std::tuple_size<type>::value>::call(f, std::forward<Tuple>(t));
}

template <typename R, typename... Args>
class Task
{

    using FT = R (*)(Args...);

public:
    Task(uint32_t freq_call, FT fn, Args &...args) : freq_call_(freq_call), m_fn(fn), args_(std::forward_as_tuple(args...)) {}

    R operator()()
    {
        return call(m_fn, args_);
    }

    R operator()(Args... args)
    {

        return call(m_fn, std::make_tuple(args...));
    }

    uint32_t getfreq()
    {
        return freq_call_;
    }

private:
    uint32_t freq_call_ = 0;
    FT m_fn;
    std::tuple<Args &...> args_;
};

template <typename R, typename... Args>
class DispatcherTask
{
    std::vector<Task<R, Args...>> functors;

public:
    void registerFunc(Task<R, Args...> functor)
    {
        functors.emplace_back(functor);
    }

    void operator()()
    {
        for (auto &func : functors)
        {
            func();
        }
    }

    void operator()(uint32_t freq_count)
    {
        for (auto &func : functors)
        {
            if (freq_count % func.getfreq() == 0)
            {
                func();
            }
        }
    }
};

class DispatcherAnyTask
{
public:
    template <typename R, typename... Args>
    void registerFunc(Task<R, Args...> functor)
    {

        held_.emplace_back(std::make_unique<holder<R, Args...>>(functor));
    }

    void operator()()
    {
        for (auto &hld : held_)
        {
            hld->run();
        }
    }

    ~DispatcherAnyTask() = default;

private:
    struct base_holder
    {
        virtual ~base_holder() = default;

        virtual void run() = 0;
    };

    template <typename R, typename... Args>
    struct holder : base_holder
    {
        holder(Task<R, Args...> functor) : functor_(functor)
        {
        }

        void run() override
        {
            functor_();
        }

        ~holder() override = default;

        Task<R, Args...> functor_;
    };

    std::vector<std::unique_ptr<base_holder>> held_;
};

template <typename F, typename... Args>
struct DeferTask
{
    DeferTask(F f, Args &&...args) : f_(f), args_(std::make_tuple(std::forward<Args>(args)...))
    {
    }
    F f_;
    std::tuple<Args...> args_;

    ~DeferTask()
    {
        try
        {
            call(f_, args_);
        }
        catch (...)
        {
        }
    }
};

template <typename F, typename... Args>
DeferTask<F, Args...> make_deferred(F f, Args &&...args)
{
    return DeferTask<F, Args...>(f, std::forward<Args>(args)...);
}