// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_ENVIRONMENT_HPP_
#define BOOST_PROCESS_ENVIRONMENT_HPP_

#include <boost/process/detail/config.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/iterator/transform_iterator.hpp>

#if defined(BOOST_POSIX_API)
#include <boost/process/detail/posix/environment.hpp>
#elif defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/environment.hpp>
#endif

namespace boost { namespace process {

namespace detail {

template<typename Char, typename Environment>
struct const_entry
{
    typedef Char value_type;
    typedef const value_type * pointer;
    typedef std::basic_string<value_type> string_type;
    typedef boost::iterator_range<pointer> range;
    typedef Environment environment_t;

    std::vector<string_type> to_vector() const
    {
        if (_data == nullptr)
            return std::vector<string_type>();
        std::vector<string_type> data;
        auto str = std::string(_data);
        struct splitter
        {
            bool operator()(wchar_t w) const {return w == L';';}
            bool operator()(char c)    const {return c == ';';}
        } s;
        boost::split(data, _data, s);
        return std::move(data);
    }
    string_type to_string()              const
    {
        if (_data != nullptr)
            return string_type(_data);
        else
            return string_type();
    }
    string_type get_name() const {return string_type(_name.begin(), _name.end());}
    const_entry(string_type&& name, pointer data, environment_t & env) :
        _name(std::move(name)), _data(data), _env(&env) {}

    const_entry(string_type &&name, environment_t & env) :
        _name(std::move(name)), _data(nullptr), _env(&env) {}
    const_entry(const const_entry&) = default;
    const_entry& operator=(const const_entry&) = default;

    void reload()
    {
        auto p = _env->find(_name);
        if (p == _env->end())
            _data = nullptr;
        else
            _data = p->_data;
        this->_env->reload();

    }
    bool empty() const
    {
        return _data == nullptr;
    }
protected:
    string_type _name;
    pointer _data;
    environment_t * _env;
};

template<typename Char, typename Environment>
struct entry : const_entry<Char, Environment>
{
    using father = const_entry<Char, Environment>;
    using string_type   = typename father::string_type;
    using pointer       = typename father::pointer;
    using environment_t = typename father::environment_t;

    entry(string_type&& name, pointer data, environment_t & env) :
        father(std::move(name), data, env) {}

    entry(string_type &&name, environment_t & env) :
        father(std::move(name), env) {}

    entry(const entry&) = default;
    entry& operator=(const entry&) = default;

    void assign(const string_type &value)
    {
        this->_env->set(this->_name, value);
        this->reload();
    }
    void assign(const std::vector<string_type> &value)
    {
        string_type data;
        for (auto &v : value)
        {
            if (&v != &value.front())
                data += ';';
            data += v;
        }
        this->_env->set(this->_name, data);
        this->_env->reload();
    }
    void append(const string_type &value)
    {
        string_type st = this->_data;
        if (st.empty())
            this->_env->set(this->_name, value);
        else
            this->_env->set(this->_name, st + ';' + value);
        this->reload();
    }
    void clear()
    {
        this->_env->reset(this->_name);
        this->_env->reload();
        this->_data = nullptr;
    }
    entry &operator=(const string_type & value)
    {
        assign(value);
        return *this;
    }
    entry &operator=(const std::vector<string_type> & value)
    {
        assign(value);
        return *this;
    }
    entry &operator+=(const string_type & value)
    {
        append(value);
        return *this;
    }

};



template<typename Char, typename Environment>
struct make_entry
{

    Environment &env;
    make_entry(Environment & env) : env(env) {};
    entry<Char, Environment> operator()(const Char* data) const
    {
        auto p = data;
        while ((*p != equal_sign<Char>) && (*p != null_char<Char>))
                p++;
        auto name = std::basic_string<Char>(data, p);
        p++; //go behind equal sign

        return entry<Char, Environment>(std::move(name), p, env);
    }
};

template<typename Char, typename Environment>
struct make_const_entry
{

    Environment &env;
    make_const_entry(Environment & env) : env(env) {};
    const_entry<Char, Environment> operator()(const Char* data) const
    {
        auto p = data;
        while ((*p != equal_sign<Char>) && (*p != null_char<Char>))
                p++;
        auto name = std::basic_string<Char>(data, p);
        p++; //go behind equal sign

        return const_entry<Char, Environment>(std::move(name), p, env);
    }
};

}

template<typename Char, template <class> class Implementation = detail::api::native_environment_impl>
class basic_environment : public Implementation<Char>
{
    Char** _get_end() const
    {
        auto p = this->_env_impl;
        while (*p != nullptr)
            p++;

        return p;
    }
public:
    using string_type = std::basic_string<Char>;
    using implementation_type = Implementation<Char>;
    using base_type = basic_environment<Char, Implementation>;
    using       entry_maker = detail::make_entry<Char, base_type>;
    using entry_type        = detail::entry<Char, base_type>;
    using const_entry_type  = detail::const_entry<Char, base_type>;
    using const_entry_maker = detail::make_const_entry<Char, base_type>;

    friend       entry_type;
    friend const_entry_type;

    using iterator        = boost::transform_iterator<      entry_maker, Char**>;
    using const_iterator  = boost::transform_iterator<const_entry_maker, Char**>;
    using size_type       = std::size_t;

    iterator       begin()        {return       iterator(this->_env_impl,       entry_maker(*this));}
    const_iterator begin()  const {return const_iterator(this->_env_impl, const_entry_maker(*this));}
    const_iterator cbegin() const {return const_iterator(this->_env_impl, const_entry_maker(*this));}

    iterator       end()        {return       iterator(_get_end(),       entry_maker(*this));}
    const_iterator end()  const {return const_iterator(_get_end(), const_entry_maker(*this));}
    const_iterator cend() const {return const_iterator(_get_end(), const_entry_maker(*this));}

    iterator        find( const string_type& key )
    {
        auto p = this->_env_impl;
        auto st1 = key + equal_sign<Char>;
        while (*p != nullptr)
        {
            if (std::equal(st1.begin(), st1.end(), *p))
                break;
            p++;
        }
        return iterator(p, entry_maker(*this));
    }
    const_iterator  find( const string_type& key ) const
    {
        auto p = this->_env_impl;
        auto st1 = key + equal_sign<Char>;
        while (*p != nullptr)
        {
            if (std::equal(st1.begin(), st1.end(), *p))
                break;
            p++;
        }
        return const_iterator(p, const_entry_maker(*this));
    }

    std::size_t count(const string_type & st) const
    {
        auto p = this->_env_impl;
        auto st1 = st + equal_sign<Char>;
        while (*p != nullptr)
        {
            if (std::equal(st1.begin(), st1.end(), *p))
                break;
            p++;
        }
        return 0u;
    }
    void erase(const string_type & id)
    {
        implementation_type::reset(id);
    }
    std::pair<iterator,bool> emplace(const string_type & id, const string_type & value)
    {
        auto p = this->_env_impl;
        auto st1 = id + equal_sign<Char>;
        auto f = find(id);
        if (f != end())
        {
            implementation_type::set(id, value);
            return std::pair<iterator, bool>(find(id), true);

        }
        else
            return std::pair<iterator, bool>(f, true);
    }
    using implementation_type::implementation_type;
    using implementation_type::operator=;
    using native_handle_type = typename implementation_type::native_handle_type;
    using implementation_type::native_handle;
    //copy ctor if impl is copy-constructible
    bool empty()
    {
        return this->_env_impl == nullptr;
    }
    std::size_t size() const
    {
        return (_get_end() - this->_env_impl);
    }
    void clear()
    {
        for (auto && i : *this)
            implementation_type::reset(i.get_name());
        this->reload();
    }

    entry_type  at( const string_type& key )
    {
        auto f = find(key);
        if (f== end())
            throw std::out_of_range(key + " not found");
        return *f;
    }
    const_entry_type at( const string_type& key ) const
    {
        auto f = find(key);
        if (f== end())
            throw std::out_of_range(key + " not found");
        return *f;
    }
    entry_type operator[](const string_type & key)
    {
        auto p = find(key);
        if (p != end())
            return *p;

        return entry_type(string_type(key), *this);
    }
};



typedef basic_environment<char,    detail::api::native_environment_impl>   native_environment;
typedef basic_environment<wchar_t, detail::api::native_environment_impl>  native_wenvironment;

typedef basic_environment<char,    detail::api::basic_environment_impl>   environment;
typedef basic_environment<wchar_t, detail::api::basic_environment_impl>  wenvironment;


namespace this_process
{

inline int   get_id()                   { return detail::api::get_id();}
inline void* native_handle()            { return detail::api::native_handle();}
inline native_environment environment() { return native_environment(); }
inline std::vector<std::string> path()  { return native_environment()["PATH"].to_vector();}
inline std::string pwd()                { return native_environment()["PWD"].to_string();}
inline std::string cwd()                { return native_environment()["CWD"].to_string();}

}


}

namespace this_process = ::boost::process::this_process;

}
#endif /* INCLUDE_BOOST_PROCESS_DETAIL_ENVIRONMENT_HPP_ */
