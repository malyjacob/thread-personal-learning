#ifndef __PACKAGE_HPP_
#define __PACKAGE_HPP_

#include <concepts>

template <class T>
concept NoVoid = !std::is_void_v<T>;

template <class T>
concept YesVoid = std::is_void_v<T>;

template <class...>
struct _NV;

template <NoVoid T>
struct _NV<T> : public std::true_type
{
};

template <YesVoid T>
struct _NV<T> : public std::false_type
{
};

template <class T1, class T2>
struct _NV<T1, T2>
{
    static constexpr bool value = std::__and_<_NV<T1>, _NV<T2>>::value;
};

template <class T1, class T2, class... Args>
struct _NV<T1, T2, Args...>
{
    static constexpr bool value = std::__and_<_NV<T1>, _NV<T2, Args...>>::value;
};

template <class... Args>
concept AllNoVoid = _NV<Args...>::value;

template <class T, class R, class... Args>
concept PCallable = requires(T t, Args... args)
{
    {
        t(args...)
        } -> std::same_as<R>;
}
&&AllNoVoid<Args...>;

template <class T, class R, class... Args>
concept PFunction = PCallable<T, R, Args...> && std::is_function_v<T>;

template <class T, class R, class... Args>
concept PFunctor = PCallable<T, R, Args...> && std::is_class_v<T>;

template <class...>
class __Base_Pkg;


template <class T>
requires std::is_class_v<T>
class __Base_Pkg<T>
{
protected:
    typedef T Fn;
    Fn _fn;

    __Base_Pkg() = default;
    template <NoVoid First, NoVoid... Rest>
    __Base_Pkg(First f, Rest... res) : _fn(f, res...) {}
};

template <class R, class... Args>
class __Base_Pkg<R(Args...)>
{
protected:
    using Fn = R (*)(Args...);
    Fn _fn;

    __Base_Pkg() = default;
    explicit __Base_Pkg(Fn fn) : _fn(fn) {}
};

template <class...>
class Calculator;

template <class F, NoVoid R, NoVoid... Args>
requires PFunctor<F, R, Args...>
class Calculator<F, R, Args...> : public __Base_Pkg<F>
{
public:
    typedef __Base_Pkg<F> Base;
    typedef R return_type;
    typedef typename Base::Fn functor_type;

    Calculator() : Base() {}

    template <NoVoid First, NoVoid... Rest>
    Calculator(First f, Rest... res) : Base(f, res...) {}

    Calculator(const Calculator &) = delete;
    Calculator &operator=(const Calculator &) = delete;

    return_type operator()(Args... args) { return Base::_fn(args...); }

    template <NoVoid... Rest>
    static Calculator init(Rest... res)
    {
        return Calculator(res...);
    }
};

template <class F, NoVoid... Args>
requires PFunctor<F, void, Args...>
class Calculator<F, Args...> : public __Base_Pkg<F>
{
public:
    typedef __Base_Pkg<F> Base;
    typedef typename Base::Fn functor_type;
    using return_type = void;

    Calculator() : Base() {}

    template <NoVoid First, NoVoid... Rest>
    Calculator(First f, Rest... res) : Base(f, res...) {}

    Calculator(const Calculator &) = delete;
    Calculator &operator=(const Calculator &) = delete;

    return_type operator()(Args... args) { Base::_fn(args...); }

    template <NoVoid... Rest>
    static Calculator init(Rest... res)
    {
        return Calculator(res...);
    }
};

template <NoVoid R, NoVoid... Args>
class Calculator<R(Args...)> : public __Base_Pkg<R(Args...)>
{
public:
    using Base = __Base_Pkg<R(Args...)>;
    using functor_type = typename Base::Fn;
    using return_type = R;

    Calculator() = delete;
    Calculator(const Calculator &) = delete;
    Calculator &operator=(const Calculator &) = delete;

    Calculator(functor_type fn) : Base(fn) {}

    return_type operator()(Args... args) { return Base::_fn(args...); }

    static Calculator init(functor_type fn)
    {
        return Calculator(fn);
    }
};

template <NoVoid... Args>
class Calculator<void(Args...)> : __Base_Pkg<void(Args...)>
{
public:
    using Base = __Base_Pkg<void(Args...)>;
    using functor_type = typename Base::Fn;
    using return_type = void;

    Calculator() = delete;
    Calculator(const Calculator &) = delete;
    Calculator &operator=(const Calculator &) = delete;

    Calculator(functor_type fn) : Base(fn) {}

    return_type operator()(Args... args) { Base::_fn(args...); }

    static Calculator init(functor_type fn)
    {
        return Calculator(fn);
    }
};

// struct package
// {
//     int operator()(int id)
//     {
//         std::cout << "package..." << id << std::endl;
//         return id;
//     }
//     void operator()(const char *nm, int id) { std::cout << "package..." << nm << id << std::endl; }
// };

// int test(int id)
// {
//     std::cout << "using test..." << std::endl;
//     return id;
// }

// void test_v(int id)
// {
//     std::cout << "using test..." << id << std::endl;
// }

// int main()
// {
//     Calculator<package, int, int> cal1;
//     cal1(12);
//     auto cal2 = Calculator<package, int, int>::init();
//     cal2(13);
//     Calculator<package, const char *, int> cal3;
//     cal3("cal", 3);
//     auto cal4 = Calculator<package, const char *, int>::init();
//     cal4("cal", 4);

//     Calculator<int(int)> cal5([](int i) -> int
//                               { std::cout << "cal..." << i << std::endl; return i; });
//     cal5(5);

//     auto cal6 = Calculator<int(int)>::init(test);
//     cal6(1);

//     Calculator<void(int)> cal7([](int id) -> void { std::cout << "cal..." << id << std::endl; });
//     cal7(7);

//     auto cal8 = Calculator<void(int)>::init(test_v);
//     cal8(8);
// }

#endif