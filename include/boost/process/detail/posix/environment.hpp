// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_POSIX_ENVIRONMENT_HPP_
#define BOOST_PROCESS_DETAIL_POSIX_ENVIRONMENT_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <boost/process/detail/config.hpp>
#include <algorithm>
#include <cstdlib>


namespace boost { namespace process { namespace detail { namespace posix {

template<typename Char>
struct native_environment_impl
{
    
public:
    using char_type = Char;
    using pointer_type = const char_type*;
    using string_type = std::basic_string<char_type>;
    using native_handle_type = char **;

    void reload() {}

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
    native_handle_type _env_impl = environ;

    native_handle_type native_handle() const {return environ;}
};

template<typename Char>
inline auto native_environment_impl<Char>::get(const pointer_type id) -> string_type
{
    return getenv(id);
}

template<typename Char>
inline void native_environment_impl<Char>::set(const pointer_type id, const pointer_type value)
{
    auto val = std::string(id) + "=" + value;
    auto res = setenv(id, value, true);
    if (res != 0)
        boost::process::detail::throw_last_error();
}

template<typename Char>
inline void  native_environment_impl<Char>::reset(const pointer_type id)
{
    auto res = unsetenv(id);
    if (res != 0)
        boost::process::detail::throw_last_error();
}



template<typename Char>
struct basic_environment_impl
{
    std::vector<std::basic_string<Char>> _data {};
    static std::vector<Char*> _load_var(std::vector<std::basic_string<Char>> & data);
    std::vector<Char*> _env_arr{_load_var(_data)};
public:
    using char_type = Char;
    using pointer_type = const char_type*;
    using string_type = std::basic_string<char_type>;
    using native_handle_type = Char**;
    void reload()
    {
        _env_arr = _load_var(_data);
        _env_impl = _env_arr.data();
    }

    string_type get(const pointer_type id) {return get(string_type(id));}
    void        set(const pointer_type id, const pointer_type value) {set(string_type(id), value);}
    void      reset(const pointer_type id)  {reset(string_type(id));}

    string_type get(const string_type & id);
    void        set(const string_type & id, const string_type & value);
    void      reset(const string_type & id);

    basic_environment_impl(const native_environment_impl<Char> & nei);
    basic_environment_impl() = default;
    basic_environment_impl(const basic_environment_impl& rhs)
        : _data(rhs._data)
    {

    }
    basic_environment_impl(basic_environment_impl && ) = default;
    basic_environment_impl & operator=(const basic_environment_impl& rhs)
    {
        _data = rhs._data;
        _env_arr = _load_var(_data);
        _env_impl = &*_env_arr.begin();
        return *this;
    }
    basic_environment_impl & operator=(basic_environment_impl && ) = default;

    Char ** _env_impl = &*_env_arr.data();

    native_handle_type native_handle() const {return &_data.front();}
};


template<typename Char>
basic_environment_impl<Char>::basic_environment_impl(const native_environment_impl<Char> & nei)
{
    auto beg = nei.native_handle();

    auto end = beg;
    while (*end != nullptr)
        end++;
    this->_data.assign(beg, end);
    reload();
}


template<typename Char>
inline auto basic_environment_impl<Char>::get(const string_type &id) -> string_type
{
    auto itr = std::find_if(_data.begin(), _data.end(), 
            [&](const string_type & st)
            {
                if (st.size() <= id.size())
                    return false;
                return std::equal(id.begin(), id.end(), st.begin()) && (st[id.size()] == '=');
            }
        );

    if (itr == _data.end())
    {
        return "";
    }
    else return
        itr->data() + id.size(); //id=Thingy -> +2 points to T
}

template<typename Char>
inline void basic_environment_impl<Char>::set(const string_type &id, const string_type &value)
{
    auto itr = std::find_if(_data.begin(), _data.end(), 
        [&](const string_type & st)
        {
            if (st.size() <= id.size())
                return false;
            return std::equal(id.begin(), id.end(), st.begin()) && (st[id.size()] == '=');
        }
    );

    if (itr != _data.end())
    {
        *itr = id + '=' + value;
    }
    else 
    {
        _data.push_back(id + '=' + value);
    }

    reload();
}

template<typename Char>
inline void  basic_environment_impl<Char>::reset(const string_type &id)
{
    auto itr = std::find_if(_data.begin(), _data.end(), 
        [&](const string_type & st)
        {
            if (st.size() <= id.size())
                return false;
            return std::equal(id.begin(), id.end(), st.begin()) && (st[id.size()] == '=');
        }
    );
    if (itr != _data.end())
    {
        _data.erase(itr);//and remove it    
    }
    
    reload();


}

template<typename Char>
std::vector<Char*> basic_environment_impl<Char>::_load_var(std::vector<std::basic_string<Char>> & data)
{
    std::vector<Char*> ret;
    ret.reserve(data.size() +1);

    for (auto & val : data)
        ret.push_back(&val.front());

    ret.push_back(nullptr);
    return ret;
}




typedef int native_handle_t;

inline int  get_id()        {return getpid(); }
inline int native_handle()  {return getpid(); }

}

}
}
}




#endif /* BOOST_PROCESS_DETAIL_WINDOWS_ENV_STORAGE_HPP_ */
