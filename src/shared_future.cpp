#include <iostream>
#include <future>
#include <chrono>
#include <mutex>
#include <condition_variable>

std::mutex mutex;

std::condition_variable cv;

bool flag = false;
bool flag1 = false;
bool flag2 = false;


int main()
{
    std::promise<int> ready_promise;
    std::promise<void> t1_ready_promise, t2_ready_promise;
    std::shared_future<int> ready_future(ready_promise.get_future());
    std::chrono::time_point<std::chrono::high_resolution_clock> start;

    auto fun1 = [&, ready_future]() -> std::chrono::duration<double, std::milli>
    {
        t1_ready_promise.set_value();
        ready_future.wait();
        int r_f = ready_future.get();
        std::unique_lock<std::mutex> lck(mutex);
        cv.wait(lck, [&]()->bool {return flag;});
        std::cout << "in thread 1, ready_future result is: " << r_f << std::endl;
        flag1 = true;
        lck.unlock();
        cv.notify_one();
        return std::chrono::high_resolution_clock::now() - start;
    };

    auto fun2 = [&, ready_future]() -> std::chrono::duration<double, std::milli>
    {
        t2_ready_promise.set_value();
        ready_future.wait();
        int r_f = ready_future.get();
        std::unique_lock<std::mutex> lck(mutex);
        cv.wait(lck, [&]()->bool {return flag;});
        std::cout << "in thread 2, ready_future result is: " << r_f << std::endl;
        flag2 = true;
        lck.unlock();
        cv.notify_one();
        return std::chrono::high_resolution_clock::now() - start;
    };

    auto result1 = std::async(std::launch::async, fun1);
    auto result2 = std::async(std::launch::async, fun2);

    std::cout << "waiting..." << std::endl;
    start = std::chrono::high_resolution_clock::now();

    t1_ready_promise.get_future().wait();
    t2_ready_promise.get_future().wait();


    ready_promise.set_value(100);
    int r_f = ready_future.get();
    std::unique_lock<std::mutex> lck(mutex);
    std::cout << "in main thread, ready_future result is: " << r_f << std::endl;
    flag = true;
    cv.wait(lck, [&]()->bool {return flag1 && flag2; });
    std::cout << "thread 1 received the signal "
              << result1.get().count() << " ms after start\n"
              << "thread 2 received the signal "
              << result2.get().count() << " ms after start\n";
}