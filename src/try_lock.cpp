#include <chrono>
#include <thread>
#include <mutex>
#include <iostream>


std::mutex cout_mutex;

int main()
{
    int foo_count = 0;
    std::mutex foo_mutex;
    int bar_count = 0;
    std::mutex bar_mutex;
    bool done = false;
    std::mutex done_mutex;
    int overall_count = 0;

    auto increment = [](int &counter, std::mutex &m, const char *desc)
    {
        for(int i = 0; i < 10; i++)
        {
            std::unique_lock<std::mutex> lock(m);
            ++counter;
            cout_mutex.lock();
            std::cout << desc << ": " << counter << std::endl;
            cout_mutex.unlock();
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    };

    auto get_all = [&]()
    {
        done_mutex.lock();
        while(!done)
        {
            done_mutex.unlock();
            int result = std::try_lock(foo_mutex, bar_mutex);
            if(result == -1)
            {
                overall_count += foo_count + bar_count;
                foo_count = 0;
                bar_count = 0;
                cout_mutex.lock();
                std::cout << "overall: " << overall_count << std::endl;
                cout_mutex.unlock();
                foo_mutex.unlock();
                bar_mutex.unlock();
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
            done_mutex.lock();
        }
        done_mutex.unlock();
    };

    std::thread increment_foo(increment, std::ref(foo_count), std::ref(foo_mutex), "foo");
    std::thread increment_bar(increment, std::ref(bar_count), std::ref(bar_mutex), "bar");
    std::thread update_overall(get_all);

    increment_foo.join();
    increment_bar.join();
    done_mutex.lock();
    done = true;
    done_mutex.unlock();
    update_overall.join();

    std::cout << "Done processing\n"
              << "foo: " << foo_count << std::endl
              << "bar: " << bar_count << std::endl
              << "overall: " << overall_count << std::endl;
}