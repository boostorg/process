// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_WINDOWS_ENV_STORAGE_HPP_
#define BOOST_PROCESS_DETAIL_WINDOWS_ENV_STORAGE_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <boost/detail/winapi/environment.hpp>
#include <boost/process/detail/config.hpp>
#include <boost/detail/winapi/get_current_process.hpp>
#include <boost/detail/winapi/get_current_process_id.hpp>

namespace boost { namespace process { namespace detail { namespace windows {

template<typename Char>
class native_environment_impl
{
    static void _deleter(Char* p) {boost::detail::winapi::free_environment_strings(p);};
    std::unique_ptr<Char[], void(*)(Char*)> _buf{boost::detail::winapi::get_environment_strings<Char>(), &native_environment_impl::_deleter};
    std::vector<Char*> _load_var(Char* p);
    std::vector<Char*> _env_arr{_load_var(_buf.get())};
public:
    using char_type = Char;
    using pointer_type = const char_type*;
    using string_type = std::basic_string<char_type>;
    using native_handle_type = pointer_type;
    void reload()
    {
        _buf.reset(boost::detail::winapi::get_environment_strings<Char>());
        _env_arr = _load_var(_buf.get());
        _env_impl = &*_env_arr.begin();
    }

    string_type get(const pointer_type id);
    void        set(const pointer_type id, const pointer_type value);
    void      reset(const pointer_type id);

    string_type get(const string_type & id) {return get(id.c_str());}
    void        set(const string_type & id, const string_type & value) {set(id.c_str(), value.c_str()); }
    void      reset(const string_type & id) {reset(id.c_str());}

    native_environment_impl() = default;
    native_environment_impl(const native_environment_impl& ) = delete;
    native_environment_impl(native_environment_impl && ) = default;
    native_environment_impl & operator=(const native_environment_impl& ) = delete;
    native_environment_impl & operator=(native_environment_impl && ) = default;
    Char ** _env_impl = &*_env_arr.begin();

    native_handle_type native_handle() {return _buf.get();}
};

template<typename Char>
inline auto native_environment_impl<Char>::get(const pointer_type id) -> string_type
{
    Char buf[4096];
    auto size = boost::detail::winapi::get_environment_variable(id, buf, sizeof(buf));
    if (size == 0) //failed
    {
        auto err =  boost::detail::winapi::GetLastError();
        if (err == boost::detail::winapi::ERROR_ENVVAR_NOT_FOUND_)//well, then we consider that an empty value
            return "";
        //Handle variables longer then buf.
    }
    return std::basic_string<Char>(buf, buf+size+1);
}

template<typename Char>
inline void native_environment_impl<Char>::set(const pointer_type id, const pointer_type value)
{
    boost::detail::winapi::set_environment_variable(id, value);
}

template<typename Char>
inline void  native_environment_impl<Char>::reset(const pointer_type id)
{
    boost::detail::winapi::set_environment_variable(id, nullptr);
}

template<typename Char>
std::vector<Char*> native_environment_impl<Char>::_load_var(Char* p)
{
    std::vector<Char*> ret;
    if (*p != null_char<Char>)
    {
        bool found = false;
        while (true)
        {
            p++;
            if (*p == null_char<Char>)
            {
                if (found)//found the second one
                    break;
                else
                    found = true;
            }
            else if (found)
            {
                ret.push_back(p);
                found = false;
            }
        }
    }
    p++;
    ret.push_back(nullptr);
    return ret;
}


template<typename Char>
struct basic_environment_impl
{
    std::vector<Char> _data = {null_char<Char>};
    std::vector<Char*> _load_var(Char* p);
    std::vector<Char*> _env_arr{_load_var(&*_data.begin())};
public:
    using char_type = Char;
    using pointer_type = const char_type*;
    using string_type = std::basic_string<char_type>;
    using native_handle_type = pointer_type;
    void reload()
    {
        _env_arr = _load_var(&*_data.begin());
        _env_impl = &*_env_arr.begin();
    }

    string_type get(const pointer_type id) {return get(string_type(id));}
    void        set(const pointer_type id, const pointer_type value) {set(string_type(id), value);}
    void      reset(const pointer_type id)  {reset(string_type(id));}

    string_type get(const string_type & id);
    void        set(const string_type & id, const string_type & value);
    void      reset(const string_type & id);

    basic_environment_impl(const native_environment_impl<Char> & nei);
    basic_environment_impl() = default;
    basic_environment_impl(const basic_environment_impl& ) = default;
    basic_environment_impl(basic_environment_impl && ) = default;
    basic_environment_impl & operator=(const basic_environment_impl& ) = default;
    basic_environment_impl & operator=(basic_environment_impl && ) = default;

    Char ** _env_impl = &*_env_arr.begin();

    native_handle_type native_handle() {return &*_data.begin();}
};


template<typename Char>
basic_environment_impl<Char>::basic_environment_impl(const native_environment_impl<Char> & nei)
{
    auto beg = nei.native_handle();
    auto p   = beg;
    while (*p != null_char<Char>)
    {
        p++;
        if (*p == null_char<Char>)
        {
            p++;
            if (*p == null_char<Char>)
            {
                p++;
                break;//got it
            }
        }
    }
    this->_data.assign(beg, p);
}


template<typename Char>
inline auto basic_environment_impl<Char>::get(const string_type &id) -> string_type
{
    for (auto & e : _env_impl)
        if (std::equal(id.begin(), id.end(), e))
            return string_type(e  + id.size() + 1);
    return string_type();
}

template<typename Char>
inline void basic_environment_impl<Char>::set(const string_type &id, const string_type &value)
{
    for (auto & e : _env_impl)
        if (std::equal(id.begin(), id.end(), e))
        {
            //ok, found the thingy.
            auto p = _data.begin() + (e - *&_data.begin());
            auto end = p;
            while (*end != null_char<Char>)
                end++;
            _data.erase(p, end);
        }

    auto new_val = id + equal_sign<Char> + value;
    _data.insert(_data.end(), new_val.begin(), new_val.end());
}

template<typename Char>
inline void  basic_environment_impl<Char>::reset(const string_type &id)
{
    for (auto & e : _env_impl)
        if (std::equal(id.begin(), id.end(), e))
        {
            //ok, found the thingy.
            auto p = _data.begin() + (e - *&_data.begin());
            auto end = p;
            while (*end != null_char<Char>)
                end++;
            _data.erase(p, end);
        }
}

template<typename Char>
std::vector<Char*> basic_environment_impl<Char>::_load_var(Char* p)
{
    std::vector<Char*> ret;
    if (*p != null_char<Char>)
    {
        bool found = false;
        while (true)
        {
            p++;
            if (*p == null_char<Char>)
            {
                if (found)//found the second one
                    break;
                else
                    found = true;
            }
            else if (found)
            {
                ret.push_back(p);
                found = false;
            }
        }
    }
    p++;
    ret.push_back(nullptr);
}





inline int   get_id()         {return boost::detail::winapi::GetCurrentProcessId();}
inline void* native_handle()  {return boost::detail::winapi::GetCurrentProcess(); }

}

}
}
}




#endif /* BOOST_PROCESS_DETAIL_WINDOWS_ENV_STORAGE_HPP_ */
