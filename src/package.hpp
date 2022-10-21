#ifndef __PACKAGE_HPP_
#define __PACKAGE_HPP_

#include <concepts>

template <class T>
concept __NoVoid = !std::is_void_v<T>;

template <class T>
concept __YesVoid = std::is_void_v<T>;

template <class...>
struct __NV;

template <__NoVoid T>
struct __NV<T> : public std::true_type
{
};

template <__YesVoid T>
struct __NV<T> : public std::false_type
{
};

template <class T1, class T2>
struct __NV<T1, T2>
{
    static constexpr bool value = std::__and_<__NV<T1>, __NV<T2>>::value;
};

template <class T1, class T2, class... Args>
struct __NV<T1, T2, Args...>
{
    static constexpr bool value = std::__and_<__NV<T1>, __NV<T2, Args...>>::value;
};

template <class... Args>
concept __AllNoVoid = __NV<Args...>::value;

template <class T, class R, class... Args>
concept __P_Callable = requires(T t, Args... args)
{
    {
        t(args...)
        } -> std::same_as<R>;
}
&&__AllNoVoid<Args...>;

template <class T, class R, class... Args>
concept __P_Function = __P_Callable<T, R, Args...> && std::is_function_v<T>;

template <class T, class R, class... Args>
concept __P_Functor = __P_Callable<T, R, Args...> && std::is_class_v<T>;

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
    template <__NoVoid First, __NoVoid... Rest>
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

template <class F, __NoVoid R, __NoVoid... Args>
requires __P_Functor<F, R, Args...>
class Calculator<F, R, Args...> : public __Base_Pkg<F>
{
public:
    typedef __Base_Pkg<F> base;
    typedef R return_type;
    typedef typename base::Fn functor_type;

    Calculator() : base() {}

    template <__NoVoid First, __NoVoid... Rest>
    Calculator(First f, Rest... res) : base(f, res...) {}

    Calculator(const Calculator &) = delete;
    Calculator &operator=(const Calculator &) = delete;

    return_type operator()(Args... args) { return base::_fn(args...); }

    template <__NoVoid... Rest>
    static Calculator init(Rest... res)
    {
        return Calculator(res...);
    }
};

template <class F, __NoVoid... Args>
requires __P_Functor<F, void, Args...>
class Calculator<F, Args...> : public __Base_Pkg<F>
{
public:
    typedef __Base_Pkg<F> base;
    typedef typename base::Fn functor_type;
    using return_type = void;

    Calculator() : base() {}

    template <__NoVoid First, __NoVoid... Rest>
    Calculator(First f, Rest... res) : base(f, res...) {}

    Calculator(const Calculator &) = delete;
    Calculator &operator=(const Calculator &) = delete;

    return_type operator()(Args... args) { base::_fn(args...); }

    template <__NoVoid... Rest>
    static Calculator init(Rest... res)
    {
        return Calculator(res...);
    }
};

template <__NoVoid R, __NoVoid... Args>
class Calculator<R(Args...)> : public __Base_Pkg<R(Args...)>
{
public:
    using base = __Base_Pkg<R(Args...)>;
    using functor_type = typename base::Fn;
    using return_type = R;

    Calculator() = delete;
    Calculator(const Calculator &) = delete;
    Calculator &operator=(const Calculator &) = delete;

    Calculator(functor_type fn) : base(fn) {}

    return_type operator()(Args... args) { return base::_fn(args...); }

    static Calculator init(functor_type fn)
    {
        return Calculator(fn);
    }
};

template <__NoVoid... Args>
class Calculator<void(Args...)> : __Base_Pkg<void(Args...)>
{
public:
    using base = __Base_Pkg<void(Args...)>;
    using functor_type = typename base::Fn;
    using return_type = void;

    Calculator() = delete;
    Calculator(const Calculator &) = delete;
    Calculator &operator=(const Calculator &) = delete;

    Calculator(functor_type fn) : base(fn) {}

    return_type operator()(Args... args) { base::_fn(args...); }

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