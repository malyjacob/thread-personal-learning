#include <iostream>
#include <future>
#include <thread>
#include <chrono>

int main()
{
    std::future<int> future = std::async(std::launch::async, []()
                                         {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        return 12; });

    std::cout << "waiting..." << std::endl;
    std::future_status status;

    do
    {
        status = future.wait_for(std::chrono::seconds(1));
        if (status == std::future_status::deferred)
        {
            std::cout << "deferred" << std::endl;
        }
        else if (status == std::future_status::timeout)
        {
            std::cout << "timeout" << std::endl;
        }
        else if (status == std::future_status::ready)
        {
            std::cout << "ready!" << std::endl;
        }
    } while (status != std::future_status::ready);

    std::cout << "result is: " << future.get() << std::endl;

    std::future<int> future2 = std::async(std::launch::deferred, []()
                                          {
        std::cout << "thread 2" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return 0xff; });

    status = future2.wait_for(std::chrono::seconds(2));
    if (status == std::future_status::deferred)
    {
        std::cout << "deferred" << std::endl;
        std::cout << "result is: " << future2.get() << std::endl;
        return 0;
    }
    else if (status == std::future_status::timeout)
    {
        std::cout << "timeout" << std::endl;
    }
    else if (status == std::future_status::ready)
    {
        std::cout << "result is: " << future2.get() << std::endl;
        return 0;
    }

    future2.wait();
    std::cout << "result is: " << future2.get() << std::endl;
    return 0;
}