#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <chrono>

struct Obj
{
    std::condition_variable cv;
    std::mutex cv_m;
    int i = 0;
    bool done = false;
};

void wait(Obj& obj)
{
    std::cout << "wait thread: [" << std::this_thread::get_id() << "] launch....\n";
    std::unique_lock<std::mutex> lck(obj.cv_m);
    std::cout << "waiting....\n";
    obj.cv.wait(lck, [&obj]{ return obj.i == 1; });
    std::cout << "wait thread: [" << std::this_thread::get_id()  << "]...finished waiting. i == 1\n";
    obj.done = !obj.done;
}

void signal(Obj& obj)
{
    std::cout << "signal thread: [" << std::this_thread::get_id() << "] launch....\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Notifying falsely...\n";
    obj.cv.notify_all();

    std::unique_lock<std::mutex> lck(obj.cv_m);
    obj.i = 1;
    while (!obj.done)
    {
        std::cout << "signal thread: [" << std::this_thread::get_id() << "] Notifying true change...\n";
        lck.unlock();
        obj.cv.notify_all();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        lck.lock();
    }
    
}

int main()
{
    Obj obj;
    std::thread t1(wait, std::ref(obj)), t2(wait, std::ref(obj)), t3(wait, std::ref(obj)), t4(signal, std::ref(obj));
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}
