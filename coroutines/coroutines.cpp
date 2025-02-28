#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <coroutine>

using namespace std::literals;

class Task
{
public:
    struct promise_type;
    using CoroHandle = std::coroutine_handle<promise_type>; 
    Task(CoroHandle coro_handle) : coro_handle_{coro_handle}
    {}

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    ~Task()
    {
        if (coro_handle_)
            coro_handle_.destroy();
    }

    bool resume() const
    {
        if (!coro_handle_ || coro_handle_.done())
            return false;

        coro_handle_.resume(); // return to suspended coro
 
        return !coro_handle_.done();
    }

    struct promise_type 
    {
        Task get_return_object()
        {
            return Task{ CoroHandle::from_promise(*this) };
        }

        auto initial_suspend()
        {
            return std::suspend_always{};
        }

        auto final_suspend() noexcept
        {
            return std::suspend_always{};
        }

        void unhandled_exception()
        {
            std::terminate();
        }
    };
private:
    std::coroutine_handle<promise_type> coro_handle_;
};

Task simplest_coroutine()
{
    std::cout << "simplest_coroutine has started..." << std::endl;
    
    int i = 0;
    ++i;

    co_await std::suspend_always();

    std::cout << "Step: " << i << std::endl;
    ++i;

    co_await std::suspend_always();

    std::cout << "Step: " << i << std::endl;
    ++i;

    std::cout << "end of simplest_coroutine\n";
}

TEST_CASE("first coroutine")
{
    Task task = simplest_coroutine();

    while (task.resume())
        std::cout << "Caller!!!\n";
}