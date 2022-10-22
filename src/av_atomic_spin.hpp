#ifndef __AV_ATM_SP
#define __AV_ATM_SP

#include <atomic>
#include <type_traits>

template <class T>
concept Integer = std::is_integral<T>::value;

template <Integer T>
class Flag : private std::atomic<T>
{
public:
    using base = std::atomic<T>;
    Flag() : base(0) {}
    Flag(const Flag &) = delete;
    Flag &operator=(const Flag &) = delete;

    using base::load;
    using base::operator++;
    using base::operator T;

    bool test(uint8_t expct, uint8_t rand)
    {
        return expct == uint8_t(load() % rand) ? true : false;
    }
};


template <uint8_t Rand>
class Lock
{
private:
    static Flag<uint32_t> flag;
    static Flag<uint8_t> cursor;

private:
    uint8_t id;

public:
    Lock() : id(cursor.load())
    {
        ++cursor;
        auto tmp = cursor.load();
        if(Rand < tmp || tmp == 0) throw; 
    }

    Lock(const Lock &) = delete;
    Lock &operator=(const Lock &) = delete;

    Lock(Lock&& other) : id(other.id)
    {
        other.id = Rand;
    }

    operator bool() const
    {
        return (id < Rand) ? true : false;
    }

    void lock() const
    {
        while(!flag.test(id, Rand)) {}
    }

    void unlock() const
    {
        ++flag;
    }
};

template <uint8_t Rand>
Flag<uint32_t> Lock<Rand>::flag = Flag<uint32_t>();

template <uint8_t Rand>
Flag<uint8_t> Lock<Rand>::cursor = Flag<uint8_t>();
#endif