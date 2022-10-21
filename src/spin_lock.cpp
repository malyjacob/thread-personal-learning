#define __SWITCH_SECOND

#include <iostream>
#include <atomic>
#include <thread>
#include <shared_mutex>
#include <chrono>

using namespace std;

#ifndef __SWITCH_SECOND
class CAS
{
private:
    std::atomic<bool> flag;

public:
    CAS() : flag(false) {}
    ~CAS() {}
    CAS(const CAS &s) = delete;
    CAS &operator=(const CAS &) = delete;
    void lock()
    {
        bool expect = false;
        while (!flag.compare_exchange_strong(expect, true))
        {
            expect = false;
        }
    }
    void unlock()
    {
        flag.store(false);
    }
};

int num = 0;
CAS sm;

void fun()
{
    for (int i = 0; i < 10; i++)
    {
        sm.lock();
        cout << "num:" << ++num << endl;
        sm.unlock();
    }
}
#endif

#ifdef __SWITCH_SECOND
class Flag
{
private:
    bool signal = false;
    mutable shared_mutex mutex;

public:
    Flag &set(bool desired)
    {
        mutex.lock();
        signal = desired;
        mutex.unlock();
        return *this;
    }
    explicit operator bool() const
    {
        mutex.lock_shared();
        bool tmp = signal;
        mutex.unlock_shared();
        return tmp;
    }
    friend bool compare_exchange(bool &expected, bool desired, Flag &flag)
    {
        bool tmp = bool(flag);
        if (expected == tmp)
        {
            flag.set(desired);
            return true;
        }
        else
            expected = tmp;

        return false;
    }
};

class Spin
{
private:
    Flag flag;

public:
    void lock()
    {
        bool expected = false;
        while (compare_exchange(expected, true, flag))
        {
            expected = false;
        }
    }
    void unlock()
    {
        flag.set(false);
    }
};

Spin sp;
int sum = 0;

void fun2()
{
    for (int i = 0; i < 10; i++)
    {
        sp.lock();
        cout << "[" << this_thread::get_id() << "]\tsum:" << ++sum << endl;
        sp.unlock();
        std::this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

int main()
{
    thread t1(fun2);
    thread t2(fun2);
    fun2();

    t1.join();
    t2.join();

    std::cout << "sum: " << sum << std::endl;
    return 0;
}

#else
int main()
{
    thread t1(fun);
    thread t2(fun);
    t1.join();
    t2.join();

    std::cout << "num: " << num << std::endl;
    return 0;
}
#endif
