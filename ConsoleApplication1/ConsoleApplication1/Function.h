#pragma once


//#define BC_NO_EXCEPTIONS


#include <utility>
#include <functional>


#define BC_SUBST(Arg)                           Arg

#define BC_BIND_DISAMBIGUATE2(has_args, ...)    BC_SUBST(BC_BIND_ ## has_args (__VA_ARGS__))
#define BC_BIND_DISAMBIGUATE(has_args, ...)     BC_BIND_DISAMBIGUATE2(has_args, __VA_ARGS__)

#define BC_HAS_ARGS_IMPL(TWO, ONE, N, ...)      N
#define BC_HAS_ARGS(...)                        BC_SUBST(BC_HAS_ARGS_IMPL(__VA_ARGS__, 2, 1, ERROR))

#define BC_BIND(...)                            BC_BIND_DISAMBIGUATE(BC_HAS_ARGS(__VA_ARGS__), __VA_ARGS__)

#define BC_BIND_1(fp)                           bc::detail::bind<decltype(fp), fp>()
#define BC_BIND_2(mf, ip)                       bc::detail::bind<decltype(mf), mf>(ip)


namespace bc // bicycle
{

    template <typename Signature>
    class function;


    namespace detail
    {

        template <typename Signature>
        struct function_traits;

        template <typename ReturnType, typename ...ArgumentTypes>
        struct function_traits<ReturnType(*)(ArgumentTypes...)>
        {
            //typedef ReturnType (*Signature)(ArgumentTypes...); // MS error C3522: parameter pack cannot be expanded in this context
            typedef function<ReturnType(ArgumentTypes...)> function_type;

            template <typename Signature, Signature fp>
            static ReturnType wrapper(void const*, ArgumentTypes&& ... args)
            {
                return (*fp)(std::forward<ArgumentTypes>(args)...);
            }
        };


        template <typename ReturnType, typename Class, typename ...ArgumentTypes>
        struct function_traits<ReturnType(Class::*)(ArgumentTypes...)>
        {
            //typedef ReturnType (Class::*Signature)(ArgumentTypes...); // MS error C3522: parameter pack cannot be expanded in this context
            typedef Class* class_ptr;
            typedef function<ReturnType(ArgumentTypes...)> function_type;

            template <typename Signature, Signature mf>
            static ReturnType wrapper(const void* ip, ArgumentTypes&& ... args)
            {
                Class* instance = const_cast<Class*>(static_cast<Class const*>(ip));
                return (instance->*mf)(std::forward<ArgumentTypes>(args)...);
            }
        };


        template <typename ReturnType, typename Class, typename ...ArgumentTypes>
        struct function_traits<ReturnType(Class::*)(ArgumentTypes...) const>
        {
            //typedef ReturnType (Class::*Signature)(ArgumentTypes...) const; // MS error C3522: parameter pack cannot be expanded in this context
            typedef const Class* class_ptr;
            typedef function<ReturnType(ArgumentTypes...)> function_type;

            template <typename Signature, Signature mf>
            static ReturnType wrapper(void const* ip, ArgumentTypes&& ... args)
            {
                Class const* instance = static_cast<Class const*>(ip);
                return (instance->*mf)(std::forward<ArgumentTypes>(args)...);
            }
        };


        // bind free function
        template <typename Signature, Signature fp>
        typename function_traits<Signature>::function_type bind()
        {
            typedef function_traits<Signature> traits;
            return typename traits::function_type(&traits::template wrapper<Signature, fp>, 0);
        }


        // bind member function
        template <typename Signature, Signature mf>
        typename function_traits<Signature>::function_type bind(typename function_traits<Signature>::class_ptr ip)
        {
            typedef function_traits<Signature> traits;
            return typename traits::function_type(&traits::template wrapper<Signature, mf>, ip);
        }

    }


    template <typename ReturnType, typename ...ArgumentTypes>
    class function<ReturnType(ArgumentTypes...)>
    {
        typedef ReturnType(*StaticFuncPtr)(void const*, ArgumentTypes&& ...);

    public:
        function() : func_(0), data_(0) {}
        function(StaticFuncPtr f, void const* d) : func_(f), data_(d) {}

        ReturnType operator () (ArgumentTypes... args) const
        {
#ifndef BC_NO_EXCEPTIONS
            if (!func_)
                throw std::bad_function_call();
#endif // BC_NO_EXCEPTIONS
            return (*func_)(data_, std::forward<ArgumentTypes>(args)...);
        }

        explicit operator bool() const
        {
            return 0 != func_;
        }

        bool operator == (function const& other) const
        {
            return func_ == other.func_ && data_ == other.data_;
        }

        bool operator != (function const& other) const
        {
            return !(*this == other);
        }

    private:
        StaticFuncPtr   func_;
        void const* data_;
    };

}
