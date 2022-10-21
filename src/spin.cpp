// #define __NO_LOG
#include <thread>
#include <mutex>
#include <future>
#include <iostream>

int main()
{
    long long num = 0;
    long long one = 0;
    long long two = 0;
    long long first = 0;
    long long second = 0;
    std::mutex mutex;

    auto incre = [&](long long &counter, long long &rank)
    {
        while (true)
        {
            while (!mutex.try_lock())
                ++counter;
            if (num >= 1024)
            {
                std::cout << "thread [" << std::this_thread::get_id() << "] num:\t" << num << "\tis done!" << std::endl;
                mutex.unlock();
                return;
            }
#ifndef __NO_LOG
            std::cout << "thread [" << std::this_thread::get_id() << "] num:\t" << num << "\tadding one" << std::endl;
#endif
            ++num;
            mutex.unlock();
            ++rank;
        }
    };

    std::future<void> future1 = std::async(std::launch::async, incre, std::ref(one), std::ref(first));
    std::future<void> future2 = std::async(std::launch::async, incre, std::ref(two), std::ref(second));

    future1.wait();
    future2.wait();

    std::cout << "\n""one   :\t\t" << one << "\n"
              << "two   :\t\t" << two << "\n"
              << "total :\t\t" << one + two << "\n\n"
              << "first :\t\t" << first << "\n"
              << "second:\t\t" << second << "\n"
              << "num   :\t\t" << num << std::endl;
}