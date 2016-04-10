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
#include <algorithm>


namespace boost { namespace process { namespace detail { namespace windows {

template<typename Char>
class native_environment_impl
{
    static void _deleter(Char* p) {boost::detail::winapi::free_environment_strings(p);};
    std::unique_ptr<Char[], void(*)(Char*)> _buf{boost::detail::winapi::get_environment_strings<Char>(), &native_environment_impl::_deleter};
    static std::vector<Char*> _load_var(Char* p);
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

    native_handle_type native_handle() const {return _buf.get();}
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
        ret.push_back(p);
        while ((*p != null_char<Char>) || (*(p+1) !=  null_char<Char>))
        {
            if (*p==null_char<Char>)
            {
                p++;
                ret.push_back(p);
            }
            else
                p++;
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
    static std::vector<Char*> _load_var(Char* p);
    std::vector<Char*> _env_arr{_load_var(_data.data())};
public:
    using char_type = Char;
    using pointer_type = const char_type*;
    using string_type = std::basic_string<char_type>;
    using native_handle_type = pointer_type;
    void reload()
    {
        _env_arr = _load_var(_data.data());
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
        _env_arr = _load_var(&*_data.begin());
        _env_impl = &*_env_arr.begin();
        return *this;
    }
    basic_environment_impl & operator=(basic_environment_impl && ) = default;

    Char ** _env_impl = &*_env_arr.begin();

    native_handle_type native_handle() const {return &*_data.begin();}
};


template<typename Char>
basic_environment_impl<Char>::basic_environment_impl(const native_environment_impl<Char> & nei)
{
    auto beg = nei.native_handle();
    auto p   = beg;
    while ((*p != null_char<Char>) || (*(p+1) != null_char<Char>))
        p++;

    p++; //pointing to the second nullchar
    p++; //to get the pointer behing the second nullchar, so it's end.

    this->_data.assign(beg, p);
}


template<typename Char>
inline auto basic_environment_impl<Char>::get(const string_type &id) -> string_type
{

    if (std::equal(id.begin(), id.end(), _data.begin()) && (_data[id.size()] == '='))
        return string_type(_data.data()); //null-char is handled by the string.

    std::vector<Char> seq = {'\0'}; //using a vector, because strings might cause problems with nullchars
    seq.insert(seq.end(), id.begin(), id.end());
    seq.push_back('=');

    auto itr = std::search(_data.begin(), _data.end(), seq.begin(), seq.end());

    if (itr == _data.end()) //not found
        return "";

    itr += seq.size(); //advance to the value behind the '='; the std::string will take care of finding the null-char.

    return string_type(&*itr);
}

template<typename Char>
inline void basic_environment_impl<Char>::set(const string_type &id, const string_type &value)
{
    reset(id);

    std::vector<char> insertion;

    insertion.insert(insertion.end(), id.begin(),    id.end());
    insertion.push_back('=');
    insertion.insert(insertion.end(), value.begin(), value.end());
    insertion.push_back('\0');

    _data.insert(_data.end() -1, insertion.begin(), insertion.end());

    reload();
}

template<typename Char>
inline void  basic_environment_impl<Char>::reset(const string_type &id)
{
    if (std::equal(id.begin(), id.end(), _data.begin()) && (_data[id.size()] == '='))
    {
        auto beg = _data.begin() + _data.size() + 1;
        auto end = beg;

        while (*end != '\0')
            end++;

        end++; //to point behind the last null-char

        _data.erase(beg, end); //and remove the thingy

    }

    std::vector<Char> seq = {'\0'}; //using a vector, because strings might cause problems with nullchars
    seq.insert(seq.end(), id.begin(), id.end());
    seq.push_back('=');

    auto itr = std::search(_data.begin(), _data.end(), seq.begin(), seq.end());

    if (itr == _data.end())
        return;//nothing to return if it's empty anyway...

    auto end = itr;

    while (*end != '\0')
        end++;

    end ++; //to point behind the last null-char

    _data.erase(itr, end);//and remove it
    reload();


}

template<typename Char>
std::vector<Char*> basic_environment_impl<Char>::_load_var(Char* p)
{
    std::vector<Char*> ret;
    if (*p != null_char<Char>)
    {
        ret.push_back(p);
        while ((*p != null_char<Char>) || (*(p+1) !=  null_char<Char>))
        {
            if (*p==null_char<Char>)
            {
                p++;
                ret.push_back(p);
            }
            else
                p++;
        }
    }
    p++;
    ret.push_back(nullptr);
    return ret;
}





inline int   get_id()         {return boost::detail::winapi::GetCurrentProcessId();}
inline void* native_handle()  {return boost::detail::winapi::GetCurrentProcess(); }

}

}
}
}




#endif /* BOOST_PROCESS_DETAIL_WINDOWS_ENV_STORAGE_HPP_ */
