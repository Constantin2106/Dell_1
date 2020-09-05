#pragma once
//#define DG_NO_EXCEPTIONS

#include <utility>
#include <functional>


#define DG_SUBST(Arg)                           Arg

#define DG_BIND_DISAMBIGUATE2(has_args, ...)    DG_SUBST(DG_BIND_ ## has_args (__VA_ARGS__))
#define DG_BIND_DISAMBIGUATE(has_args, ...)     DG_BIND_DISAMBIGUATE2(has_args, __VA_ARGS__)

#define DG_HAS_ARGS_IMPL(TWO, ONE, N, ...)      N
#define DG_HAS_ARGS(...)                        DG_SUBST(DG_HAS_ARGS_IMPL(__VA_ARGS__, 2, 1, ERROR))

#define DG_BIND_1(fp)                           Utils::/*detail::*/bind<decltype(fp), fp>()
#define DG_BIND_2(mf, ip)                       Utils::/*detail::*/bind<decltype(mf), mf>(ip)

#define DG_BIND(...)                            DG_BIND_DISAMBIGUATE(DG_HAS_ARGS(__VA_ARGS__), __VA_ARGS__)

namespace FunCaller
{
    template<typename Signature>
    class Caller;

    template<typename R, typename... Args>
    class Caller<R(Args...)>
    {
        using Func = R(*)(Args...);
        Func mFunc{};
        const void* mObj{};

    public:
        Caller(Func f, const void* o = nullptr) : mFunc(f), mObj(o) {}
        R operator() (Args... args) const
        {
            if (mFunc)
            {
                /*if(mObj)
                    return (mObj->*mFunc)(std::forward<Args>(args)...);*/
                return (*mFunc)(std::forward<Args>(args)...);
            }
        }
    };

    template<typename R, typename C, typename... Args>
    class Caller<R(C::*)(Args...)>
    {
        using Func = R(C::*)(Args...);
        Func mFunc{};
        const void* mObj{};

    public:
        Caller(Func f, const void* o) : mFunc(f), mObj(o) {}
        R operator() (Args... args) const
        {
            if (mFunc)
            {
                if (mObj)
                {
                    C* instance = const_cast<C*>(static_cast<C const*>(mObj));
                    return (instance->*mFunc)(std::forward<Args>(args)...);
                }
                //return (*mFunc)(std::forward<Args>(args)...);
            }
        }
    };
}
namespace Utils
{
    template <typename Signature>
    class Delegate;

    template <typename R, typename ...Args>
    class Delegate<R(Args...)>
    {
        using Function_Ptr = R(*)(void const*, Args&& ...);
        
        Function_Ptr mFunction{};
        void const* mObject{};

    public:
        Delegate() : mFunction(0), mObject(0) {}
        Delegate(Function_Ptr f, void const* d) : mFunction(f), mObject(d) {}

        R operator() (Args... args) const
        {
#ifndef DG_NO_EXCEPTIONS
            if (!mFunction)
                throw std::bad_function_call();
#endif // DG_NO_EXCEPTIONS
            return (*mFunction)(mObject, std::forward<Args>(args)...);
        }

        explicit operator bool() const
        {
            return nullptr != mFunction;
        }

        bool operator== (Delegate const& other) const
        {
            return mFunction == other.mFunction && mObject == other.mObject;
        }

        bool operator!= (Delegate const& other) const
        {
            return !(*this == other);
        }
    };

    namespace/* detail*/
    {
        template <typename Signature>
        struct function_traits;

        template <typename R, typename ...Args>
        struct function_traits<R(*)(Args...)>
        {
            //typedef R (*Signature)(Args...); // MS error C3522: parameter pack cannot be expanded in this context
            using function_type = Delegate<R(Args...)>;

            template <typename Signature, Signature fp>
            static R wrapper(void const*, Args&& ... args)
            {
                return (*fp)(std::forward<Args>(args)...);
            }
        };

        template <typename R, typename Class, typename ...Args>
        struct function_traits<R(Class::*)(Args...)>
        {
            //typedef R (Class::*Signature)(Args...); // MS error C3522: parameter pack cannot be expanded in this context
            using class_ptr = Class*;
            using function_type = Delegate<R(Args...)>;

            template <typename Signature, Signature mf>
            static R wrapper(const void* ip, Args&& ... args)
            {
                Class* instance = const_cast<Class*>(static_cast<Class const*>(ip));
                return (instance->*mf)(std::forward<Args>(args)...);
            }
        };


        template <typename R, typename Class, typename ...Args>
        struct function_traits<R(Class::*)(Args...) const>
        {
            //typedef R (Class::*Signature)(Args...) const; // MS error C3522: parameter pack cannot be expanded in this context
            using class_ptr = const Class*;
            using function_type = Delegate<R(Args...)>;

            template <typename Signature, Signature mf>
            static R wrapper(void const* ip, Args&& ... args)
            {
                Class const* instance = static_cast<Class const*>(ip);
                return (instance->*mf)(std::forward<Args>(args)...);
            }
        };


        // bind free function
        template <typename Signature, Signature fp>
        typename function_traits<Signature>::function_type bind()
        {
            using traits = function_traits<Signature>;
            return typename traits::function_type(&traits::template wrapper<Signature, fp>, 0);
        }


        // bind member function
        template <typename Signature, Signature mf>
        typename function_traits<Signature>::function_type bind(typename function_traits<Signature>::class_ptr ip)
        {
            using traits = function_traits<Signature>;
            return typename traits::function_type(&traits::template wrapper<Signature, mf>, ip);
        }

    }


}
