//
// process/environment.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Klemens D. Morgenstern (klemens dot morgenstern at gmx dot net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_V2_ENVIRONMENT_HPP
#define BOOST_PROCESS_V2_ENVIRONMENT_HPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/cstring_ref.hpp>
#include <boost/process/v2/detail/utf8.hpp>
#include <memory>
#include <numeric>

#if !defined(GENERATING_DOCUMENTATION)
#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <boost/process/v2/detail/environment_win.hpp>
#else
#include <boost/process/v2/detail/environment_posix.hpp>
#endif
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

/// Namespace for information and functions regarding the calling process.
namespace environment
{

#if defined(GENERATING_DOCUMENTATION)

/// A char traits type that reflects the OS rules for string representing environment keys.
/** Can be an alias of std::char_traits. May only be defined for `char` and `wchar_t`.
*/
tempalte<typename Char>
using key_char_traits = implementation-defined ;

/// A char traits type that reflects the OS rules for string representing environment values.
/** Can be an alias of std::char_traits. May only be defined for `char` and `wchar_t`.
*/
tempalte<typename Char>
using value_char_traits = implementation-defined ;

/// The character type used by the environment. Either `char` or `wchar_t`.
using char_type = implementation-defined ;

/// The equal character in an environment string used to separate key and value.
constexpr char_type equality_sign = implementation-defined;

/// The delimiter in environemtn lists. Commonly used by the `PATH` variable.
constexpr char_type equality_sign = implementation-defined;

/// The native handle of an environment. Note that this can be an owning pointer and is generally not thread safe.
using native_handle = implementation-defined;


#endif


/// The iterator used by a value or value_view to iterator through environments that are lists.
struct value_iterator
{
    using string_view_type  = basic_string_view<char_type, value_char_traits<char_type>>;
    using difference_type   = std::size_t;
    using value_type        = string_view_type;
    using pointer           = const string_view_type *;
    using reference         = const string_view_type & ;
    using iterator_category = std::forward_iterator_tag;

    value_iterator & operator++()
    {
        const auto delim = view_.find(delimiter);
        if (delim != string_view_type::npos)
          view_ = view_.substr(delim + 1);
        else
          view_ = view_.substr(view_.size());
        return *this;
    }

    value_iterator operator++(int)
    {
        auto last = *this;
        ++(*this);
        return last;
    }
    string_view_type operator*() const
    {
      const auto delim = view_.find(delimiter);
      if (delim == string_view_type::npos)
        return view_;
      else
        return view_.substr(0, delim);
    }
    const string_view_type operator->() const
    {
        return **this;
    }


    value_iterator() = default;
    value_iterator(const value_iterator & ) = default;
    value_iterator(string_view_type view, std::size_t offset = 0u) : view_(view.substr(offset))
    {
    }

    friend bool operator==(const value_iterator & l, const value_iterator & r) { return l.view_ == r.view_; }
    friend bool operator!=(const value_iterator & l, const value_iterator & r) { return l.view_ != r.view_; }
    friend bool operator<=(const value_iterator & l, const value_iterator & r) { return l.view_ <= r.view_; }
    friend bool operator>=(const value_iterator & l, const value_iterator & r) { return l.view_ >= r.view_; }
    friend bool operator< (const value_iterator & l, const value_iterator & r) { return l.view_ <  r.view_; }
    friend bool operator> (const value_iterator & l, const value_iterator & r) { return l.view_ >  r.view_; }

  private:
    string_view_type view_;
};

/// A view type for a key of an environment
struct key_view
{
    using value_type       = char_type;
    using traits_type      = key_char_traits<char_type>;
    using string_view_type = basic_string_view<char_type, traits_type>;
    using string_type      = std::basic_string<char_type, key_char_traits<char_type>>;

    key_view() noexcept = default;
    key_view( const key_view& p ) = default;
    key_view( key_view&& p ) noexcept = default;
    template<typename Source, 
             typename = typename std::enable_if<is_constructible<string_view_type, Source>::value>::type>
    key_view( const Source& source ) : value_(source) {}
    key_view( const char_type * p) : value_(p) {}
    key_view(       char_type * p) : value_(p) {}

    ~key_view() = default;

    key_view& operator=( const key_view& p ) = default;
    key_view& operator=( key_view&& p ) noexcept = default;
    key_view& operator=( string_view_type source )
    {
        value_ = source;
        return *this;
    }

    void swap( key_view& other ) noexcept
    {
        std::swap(value_, other.value_);
    }

    string_view_type native() const noexcept {return value_;}

    operator string_view_type() const {return native();}

    int compare( const key_view& p ) const noexcept {return value_.compare(p.value_);}
    int compare( string_view_type str ) const {return value_.compare(str);}
    int compare( const value_type* s ) const {return value_.compare(s);}

    template< class CharT, class Traits = std::char_traits<CharT>,
            class Alloc = std::allocator<CharT> >
    std::basic_string<CharT,Traits,Alloc>
    basic_string( const Alloc& alloc = Alloc()) const
    {
        return boost::process::v2::detail::conv_string<CharT, Traits>(
            value_.data(), value_.size(), alloc);
    }

    std::string       string() const {return basic_string<char>();}
    std::wstring     wstring() const {return basic_string<wchar_t>();}

    string_type native_string() const
    {
        return basic_string<char_type, key_char_traits<char_type>>();
    }

    friend bool operator==(key_view l, key_view r) { return l.value_ == r.value_; }
    friend bool operator!=(key_view l, key_view r) { return l.value_ != r.value_; }
    friend bool operator<=(key_view l, key_view r) { return l.value_ <= r.value_; }
    friend bool operator>=(key_view l, key_view r) { return l.value_ >= r.value_; }
    friend bool operator< (key_view l, key_view r) { return l.value_ <  r.value_; }
    friend bool operator> (key_view l, key_view r) { return l.value_ >  r.value_; }

    bool empty() const {return value_.empty(); }

    template< class CharT, class Traits >
    friend std::basic_ostream<CharT,Traits>&
    operator<<( std::basic_ostream<CharT,Traits>& os, const key_view& p )
    {
        os << boost::process::v2::quoted(p.basic_string<CharT,Traits>());
        return os;
    }

    template< class CharT, class Traits >
    friend std::basic_istream<CharT,Traits>&
    operator>>( std::basic_istream<CharT,Traits>& is, key_view& p )
    {
        std::basic_string<CharT, Traits> t;
        is >> boost::process::v2::quoted(t);
        p = t;
        return is;
    }
    const value_type * data() const {return value_.data(); }
    std::size_t size() const {return value_.size(); }
  private:
    string_view_type value_;
};

struct value_view
{
    using value_type       = char_type;
    using string_view_type = basic_cstring_ref<char_type, value_char_traits<char_type>>;
    using string_type      = std::basic_string<char_type, value_char_traits<char_type>>;
    using traits_type      = value_char_traits<char_type>;

    value_view() noexcept = default;
    value_view( const value_view& p ) = default;
    value_view( value_view&& p ) noexcept = default;
    template<typename Source, typename = typename std::enable_if<is_constructible<string_view_type, Source>::value>::type>
    value_view( const Source& source ) : value_(source) {}
    value_view( const char_type * p) : value_(p) {}
    value_view(       char_type * p) : value_(p) {}

    ~value_view() = default;

    value_view& operator=( const value_view& p ) = default;
    value_view& operator=( value_view&& p ) noexcept = default;
    value_view& operator=( string_view_type source )
    {
        value_ = source;
        return *this;
    }

    void swap( value_view& other ) noexcept
    {
        std::swap(value_, other.value_);
    }

    string_view_type native() const noexcept {return value_;}

    operator string_view_type() const {return native();}
    operator typename string_view_type::string_view_type() const {return value_; }

    int compare( const value_view& p ) const noexcept {return value_.compare(p.value_);}
    int compare( string_view_type str ) const {return value_.compare(str);}
    int compare( const value_type* s ) const {return value_.compare(s);}

    template< class CharT, class Traits = std::char_traits<CharT>,
            class Alloc = std::allocator<CharT> >
    std::basic_string<CharT,Traits,Alloc>
    basic_string( const Alloc& alloc = Alloc() ) const
    {
        return boost::process::v2::detail::conv_string<CharT, Traits>(
                value_.data(), value_.size(),  alloc);
    }

    std::string string() const       {return basic_string<char>();}
    std::wstring wstring() const     {return basic_string<wchar_t>();}

    string_type native_string() const
    {
        return basic_string<char_type, value_char_traits<char_type>>();
    }

    bool empty() const {return value_.empty(); }

    friend bool operator==(value_view l, value_view r) { return l.value_ == r.value_; }
    friend bool operator!=(value_view l, value_view r) { return l.value_ != r.value_; }
    friend bool operator<=(value_view l, value_view r) { return l.value_ <= r.value_; }
    friend bool operator>=(value_view l, value_view r) { return l.value_ >= r.value_; }
    friend bool operator< (value_view l, value_view r) { return l.value_ <  r.value_; }
    friend bool operator> (value_view l, value_view r) { return l.value_ >  r.value_; }


    template< class CharT, class Traits >
    friend std::basic_ostream<CharT,Traits>&
    operator<<( std::basic_ostream<CharT,Traits>& os, const value_view& p )
    {
        os << boost::process::v2::quoted(p.basic_string<CharT,Traits>());
        return os;
    }

    template< class CharT, class Traits >
    friend std::basic_istream<CharT,Traits>&
    operator>>( std::basic_istream<CharT,Traits>& is, value_view& p )
    {
        std::basic_string<CharT, Traits> t;
        is >> boost::process::v2::quoted(t);
        p = t;
        return is;
    }
    value_iterator begin() const {return value_iterator(value_.data());}
    value_iterator   end() const {return value_iterator(value_.data() , value_.size());}

    const char_type * c_str() {return value_.c_str(); }
    const value_type * data() const {return value_.data(); }
    std::size_t size() const {return value_.size(); }

  private:
    string_view_type value_;
};

struct key_value_pair_view
{
  using value_type       = char_type;
  using string_type      = std::basic_string<char_type>;
  using string_view_type = basic_cstring_ref<char_type>;
  using traits_type      = std::char_traits<char_type>;

  key_value_pair_view() noexcept = default;
  key_value_pair_view( const key_value_pair_view& p ) = default;
  key_value_pair_view( key_value_pair_view&& p ) noexcept = default;
  template<typename Source,
           typename = typename std::enable_if<is_constructible<string_view_type, Source>::value>::type>
  key_value_pair_view( const Source& source ) : value_(source) {}

  key_value_pair_view( const char_type * p) : value_(p) {}
  key_value_pair_view(       char_type * p) : value_(p) {}


  ~key_value_pair_view() = default;

  key_value_pair_view& operator=( const key_value_pair_view& p ) = default;
  key_value_pair_view& operator=( key_value_pair_view&& p ) noexcept = default;

  void swap( key_value_pair_view& other ) noexcept
  {
      std::swap(value_, other.value_);
  }

  string_view_type native() const noexcept {return value_;}

  operator string_view_type() const {return native();}
  operator typename string_view_type::string_view_type() const {return value_; }

  int compare( key_value_pair_view p ) const noexcept 
  {
      const auto c = key().compare(p.key());
      if (c != 0)
            return c;
      return value().compare(p.value());
  }
  int compare( const string_type& str ) const
  {
      return compare(key_value_pair_view(str));
  }
  int compare( string_view_type str ) const 
  {
      string_type st(str.data(), str.size());
      return compare(st);
  }
  int compare( const value_type* s ) const
  {
      return compare(key_value_pair_view(s));
  }

  template< class CharT, class Traits = std::char_traits<CharT>, class Alloc = std::allocator<CharT> >
  std::basic_string<CharT,Traits,Alloc>
  basic_string( const Alloc& alloc = Alloc()) const
  {
      return boost::process::v2::detail::conv_string<CharT, Traits>(value_.begin(), value_.size(), alloc);
  }

  std::string string() const       {return basic_string<char>();}
  std::wstring wstring() const     {return basic_string<wchar_t>();}

  string_type native_string() const
  {
    return basic_string<char_type>();
  }

  bool empty() const {return value_.empty(); }

  key_view key() const
  {
      auto eq = value_.find(equality_sign);
      if (eq == 0)
      {
          auto eq2 = value_.find(equality_sign, 1);
          if (eq2 != string_type::npos)
              eq = eq2;
      }
      const auto res = native().substr(0,  eq == string_view_type::npos ? value_.size() : eq);
      return key_view::string_view_type(res.data(), res.size());
  }
  value_view value() const
  {
      auto eq = value_.find(equality_sign);
      if (eq == 0)
      {
          auto eq2 = value_.find(equality_sign, 1);
          if (eq2 != string_type::npos)
              eq = eq2;
      }
      return environment::value_view(native().substr(eq + 1));
  }

  friend bool operator==(key_value_pair_view l, key_value_pair_view r) { return l.compare(r) == 0; }
  friend bool operator!=(key_value_pair_view l, key_value_pair_view r) { return l.compare(r) != 0; }
  friend bool operator<=(key_value_pair_view l, key_value_pair_view r) { return l.compare(r) <= 0; }
  friend bool operator>=(key_value_pair_view l, key_value_pair_view r) { return l.compare(r) >= 0; }
  friend bool operator< (key_value_pair_view l, key_value_pair_view r) { return l.compare(r) <  0; }
  friend bool operator> (key_value_pair_view l, key_value_pair_view r) { return l.compare(r) >  0; }


  template< class CharT, class Traits >
  friend std::basic_ostream<CharT,Traits>&
  operator<<( std::basic_ostream<CharT,Traits>& os, const key_value_pair_view& p )
  {
      os << boost::process::v2::quoted(p.basic_string<CharT,Traits>());
      return os;
  }

  template< class CharT, class Traits >
  friend std::basic_istream<CharT,Traits>&
  operator>>( std::basic_istream<CharT,Traits>& is, key_value_pair_view& p )
  {
      std::basic_string<CharT, Traits> t;
      is >> boost::process::v2::quoted(t);
      p = t;
      return is;
  }

  template<std::size_t Idx>
  inline auto get() const -> typename conditional<Idx == 0u, boost::process::v2::environment::key_view,
                                                             boost::process::v2::environment::value_view>::type;
  const value_type * c_str() const noexcept
  {
    return value_.data();
  }
  const value_type * data() const {return value_.data(); }
  std::size_t size() const {return value_.size(); }

 private:

  string_view_type value_;
};

template<>
inline key_view key_value_pair_view::get<0u>() const
{
    return key();
}

template<>
inline value_view key_value_pair_view::get<1u>() const
{
    return value();
}

struct key
{
    using value_type       = char_type;
    using traits_type      = key_char_traits<char_type>;
    using string_type      = std::basic_string<char_type, traits_type>;
    using string_view_type = basic_string_view<char_type, traits_type>;

    key() noexcept = default;
    key( const key& p ) = default;
    key( key&& p ) noexcept = default;
    key( const string_type& source ) : value_(source) {}
    key( string_type&& source ) : value_(std::move(source)) {}
    key( const value_type * raw ) : value_(raw) {}
    key(       value_type * raw ) : value_(raw) {}

    explicit key(key_view kv) : value_(kv.native_string()) {}


    template< class Source >
    key( const Source& source,
        decltype(source.data()) = nullptr,
        decltype(source.size()) = 0u)
        : value_(
             boost::process::v2::detail::conv_string<char_type, traits_type>(
                source.data(), source.size()))
    {
    }

    key(const typename conditional<is_same<value_type, char>::value, wchar_t, char>::type  * raw)
        : value_(boost::process::v2::detail::conv_string<char_type, traits_type>(
                raw, std::char_traits<std::decay<std::remove_pointer<decltype(raw)>::type>::type>::length(raw)))
    {
    }

    template< class InputIt >
    key( InputIt first, InputIt last)
        : key(std::basic_string<typename std::iterator_traits<std::decay_t<InputIt>>::value_type>(first, last))
    {
    }

    ~key() = default;

    key& operator=( const key& p ) = default;
    key& operator=( key&& p ) noexcept = default;
    key& operator=( string_type&& source )
    {
        value_ = std::move(source);
        return *this;
    }
    template< class Source >
    key& operator=( const Source& source )
    {
        value_ = boost::process::v2::detail::conv_string<char_type, traits_type>(source.data(), source.size());
        return *this;
    }

    key& assign( string_type&& source )
    {
        value_ = std::move(source);
        return *this;
    }
    template< class Source >
    key& assign( const Source& source )
    {
        value_ = boost::process::v2::detail::conv_string<char_type, traits_type>(source.data(), source.size());
        return *this;
    }

    template< class InputIt >
    key& assign( InputIt first, InputIt last )
    {

        return assign(std::basic_string<typename std::iterator_traits<std::decay_t<InputIt>>::value_type>(first, last));
    }

    void clear() {value_.clear();}

    void swap( key& other ) noexcept
    {
        std::swap(value_, other.value_);
    }

    const value_type* c_str() const noexcept {return value_.c_str();}
    const string_type& native() const noexcept {return value_;}
    string_view_type native_view() const noexcept {return value_;}

    operator string_type() const {return native();}
    operator string_view_type() const {return native_view();}

    int compare( const key& p ) const noexcept {return value_.compare(p.value_);}
    int compare( const string_type& str ) const {return value_.compare(str);}
    int compare( string_view_type str ) const {return -str.compare(value_);}
    int compare( const value_type* s ) const {return value_.compare(s);}

    template< class CharT, class Traits = std::char_traits<CharT>,
        class Alloc = std::allocator<CharT> >
    std::basic_string<CharT,Traits,Alloc>
    basic_string( const Alloc& alloc = Alloc()) const
    {
        return boost::process::v2::detail::conv_string<CharT, Traits>(
            value_.data(), value_.size(), alloc);
    }


    std::string string() const       {return basic_string<char>();}
    std::wstring wstring() const     {return basic_string<wchar_t>();}

    const string_type & native_string() const
    {
        return value_;
    }

    bool empty() const {return value_.empty(); }

    friend bool operator==(const key & l, const key & r) { return l.value_ == r.value_; }
    friend bool operator!=(const key & l, const key & r) { return l.value_ != r.value_; }
    friend bool operator<=(const key & l, const key & r) { return l.value_ <= r.value_; }
    friend bool operator>=(const key & l, const key & r) { return l.value_ >= r.value_; }
    friend bool operator< (const key & l, const key & r) { return l.value_ <  r.value_; }
    friend bool operator> (const key & l, const key & r) { return l.value_ >  r.value_; }

    template< class CharT, class Traits >
    friend std::basic_ostream<CharT,Traits>&
    operator<<( std::basic_ostream<CharT,Traits>& os, const key& p )
    {
        os << boost::process::v2::quoted(p.basic_string<CharT,Traits>());
        return os;
    }

    template< class CharT, class Traits >
    friend std::basic_istream<CharT,Traits>&
    operator>>( std::basic_istream<CharT,Traits>& is, key& p )
    {
        std::basic_string<CharT, Traits> t;
        is >> boost::process::v2::quoted(t);
        p = t;
        return is;
    }
    const value_type * data() const {return value_.data(); }
    std::size_t size() const {return value_.size(); }

  private:
    string_type value_;
};


template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, key>::value || std::is_same<T, key_view>::value) &&
         std::is_convertible<const U &, key_view>::value)
        ||
        ((std::is_same<U, key>::value || std::is_same<U, key_view>::value) &&
         std::is_convertible<const T & , key_view>::value),
        bool>::type
operator==(const T  &l, const U & r) { return key_view(l) == key_view(r); }

template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, key>::value || std::is_same<T, key_view>::value) &&
         std::is_convertible<const U &, key_view>::value)
        ||
        ((std::is_same<U, key>::value || std::is_same<U, key_view>::value) &&
         std::is_convertible<const T & , key_view>::value),
        bool>::type
operator!=(const T  &l, const U & r) { return key_view(l) != key_view(r); }

template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, key>::value || std::is_same<T, key_view>::value) &&
         std::is_convertible<const U &, key_view>::value)
        ||
        ((std::is_same<U, key>::value || std::is_same<U, key_view>::value) &&
         std::is_convertible<const T & , key_view>::value),
        bool>::type
operator<=(const T  &l, const U & r) { return key_view(l) <= key_view(r); }

template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, key>::value || std::is_same<T, key_view>::value) &&
         std::is_convertible<const U &, key_view>::value)
        ||
        ((std::is_same<U, key>::value || std::is_same<U, key_view>::value) &&
         std::is_convertible<const T & , key_view>::value),
        bool>::type
operator <(const T  &l, const U & r) { return key_view(l)  < key_view(r); }

template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, key>::value || std::is_same<T, key_view>::value) &&
         std::is_convertible<const U &, key_view>::value)
        ||
        ((std::is_same<U, key>::value || std::is_same<U, key_view>::value) &&
         std::is_convertible<const T & , key_view>::value),
        bool>::type
operator>=(const T  &l, const U & r) { return key_view(l) >= key_view(r); }

template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, key>::value || std::is_same<T, key_view>::value) &&
         std::is_convertible<const U &, key_view>::value)
        ||
        ((std::is_same<U, key>::value || std::is_same<U, key_view>::value) &&
         std::is_convertible<const T & , key_view>::value),
        bool>::type
operator >(const T  &l, const U & r) { return key_view(l) >  key_view(r); }


struct value
{
    using value_type       = char_type;
    using traits_type      = value_char_traits<char_type>;
    using string_type      = std::basic_string<char_type, traits_type>;
    using string_view_type = basic_cstring_ref<char_type, traits_type>;

    value() noexcept = default;
    value( const value& p ) = default;

    value( const string_type& source ) : value_(source) {}
    value( string_type&& source ) : value_(std::move(source)) {}
    value( const value_type * raw ) : value_(raw) {}
    value(       value_type * raw ) : value_(raw) {}

    explicit value(value_view kv) : value_(kv.c_str()) {}

    template< class Source >
    value( const Source& source,
           decltype(source.data()) = nullptr,
    decltype(source.size()) = 0u)
    : value_(boost::process::v2::detail::conv_string<char_type, traits_type>(
        source.data(), source.size()))
    {
    }

    value(const typename conditional<is_same<value_type, char>::value, wchar_t, char>::type  * raw)
            : value_(boost::process::v2::detail::conv_string<char_type, traits_type>(
            raw, std::char_traits<std::decay<std::remove_pointer<decltype(raw)>::type>::type>::length(raw)))
    {
    }

    template< class InputIt >
    value( InputIt first, InputIt last)
            : value(std::basic_string<typename std::iterator_traits<std::decay_t<InputIt>>::value_type>(first, last))
    {
    }

    ~value() = default;

    value& operator=( const value& p ) = default;
    value& operator=( value&& p ) noexcept = default;
    value& operator=( string_type&& source )
    {
        value_ = std::move(source);
        return *this;
    }
    template< class Source >
    value& operator=( const Source& source )
    {
        value_ = boost::process::v2::detail::conv_string<char_type, traits_type>(
            source.data(), source.size);
        return *this;
    }

    value& assign( string_type&& source )
    {
        value_ = std::move(source);
        return *this;
    }
    template< class Source >
    value& assign( const Source& source )
    {
      value_ = boost::process::v2::detail::conv_string<char_type, traits_type>(
          source.data(), source.size());
      return *this;
    }

    template< class InputIt >
    value& assign( InputIt first, InputIt last )
    {
        return assign(std::basic_string<typename std::iterator_traits<std::decay_t<InputIt>>::value_type>(first, last));
    }

    void push_back(const value & sv)
    {
        (value_ += delimiter) += sv;
    }

    void clear() {value_.clear();}

    void swap( value& other ) noexcept
    {
        std::swap(value_, other.value_);
    }

    const value_type* c_str() const noexcept {return value_.c_str();}
    const string_type& native() const noexcept {return value_;}
    string_view_type native_view() const noexcept {return value_;}

    operator string_type() const {return native();}
    operator string_view_type() const {return native_view();}
    operator typename string_view_type::string_view_type() const {return value_; }

    int compare( const value& p ) const noexcept {return value_.compare(p.value_);}
    int compare( const string_type& str ) const {return value_.compare(str);}
    int compare( string_view_type str ) const {return -str.compare(value_);}
    int compare( const value_type* s ) const {return value_.compare(s);}

    template< class CharT, class Traits = std::char_traits<CharT>,
            class Alloc = std::allocator<CharT> >
    std::basic_string<CharT,Traits,Alloc>
    basic_string( const Alloc& alloc = Alloc()) const
    {
        return boost::process::v2::detail::conv_string<CharT, Traits>(
            value_.data(), value_.size(),alloc);
    }

    std::string string() const       {return basic_string<char>();}
    std::wstring wstring() const     {return basic_string<wchar_t>();}


    const string_type & native_string() const
    {
        return value_;
    }

    bool empty() const {return value_.empty(); }

    friend bool operator==(const value & l, const value & r) { return l.value_ == r.value_; }
    friend bool operator!=(const value & l, const value & r) { return l.value_ != r.value_; }
    friend bool operator<=(const value & l, const value & r) { return l.value_ <= r.value_; }
    friend bool operator>=(const value & l, const value & r) { return l.value_ >= r.value_; }
    friend bool operator< (const value & l, const value & r) { return l.value_ <  r.value_; }
    friend bool operator> (const value & l, const value & r) { return l.value_ >  r.value_; }

    template< class CharT, class Traits >
    friend std::basic_ostream<CharT,Traits>&
    operator<<( std::basic_ostream<CharT,Traits>& os, const value& p )
    {
        os << boost::process::v2::quoted(p.basic_string<CharT,Traits>());
        return os;
    }

    template< class CharT, class Traits >
    friend std::basic_istream<CharT,Traits>&
    operator>>( std::basic_istream<CharT,Traits>& is, value& p )
    {
        std::basic_string<CharT, Traits> t;
        is >> boost::process::v2::quoted(t);
        p = t;
        return is;
    }

    value_iterator begin() const {return value_iterator(value_.data());}
    value_iterator   end() const {return value_iterator(value_.data(), value_.size());}
    const value_type * data() const {return value_.data(); }
    std::size_t size() const {return value_.size(); }

  private:
    string_type value_;
};



template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, value>::value || std::is_same<T, value_view>::value) &&
         std::is_convertible<const U &, value_view>::value)
        ||
        ((std::is_same<U, value>::value || std::is_same<U, value_view>::value) &&
         std::is_convertible<const T & , value_view>::value),
        bool>::type
operator==(const T  &l, const U & r) { return value_view(l) == value_view(r); }

template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, value>::value || std::is_same<T, value_view>::value) &&
         std::is_convertible<const U &, value_view>::value)
        ||
        ((std::is_same<U, value>::value || std::is_same<U, value_view>::value) &&
         std::is_convertible<const T & , value_view>::value),
        bool>::type
operator!=(const T  &l, const U & r) { return value_view(l) != value_view(r); }

template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, value>::value || std::is_same<T, value_view>::value) &&
         std::is_convertible<const U &, value_view>::value)
        ||
        ((std::is_same<U, value>::value || std::is_same<U, value_view>::value) &&
         std::is_convertible<const T & , value_view>::value),
        bool>::type
operator<=(const T  &l, const U & r) { return value_view(l) <= value_view(r); }

template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, value>::value || std::is_same<T, value_view>::value) &&
         std::is_convertible<const U &, value_view>::value)
        ||
        ((std::is_same<U, value>::value || std::is_same<U, value_view>::value) &&
         std::is_convertible<const T & , value_view>::value),
        bool>::type
operator <(const T  &l, const U & r) { return value_view(l)  < value_view(r); }

template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, value>::value || std::is_same<T, value_view>::value) &&
         std::is_convertible<const U &, value_view>::value)
        ||
        ((std::is_same<U, value>::value || std::is_same<U, value_view>::value) &&
         std::is_convertible<const T & , value_view>::value),
        bool>::type
operator>=(const T  &l, const U & r) { return value_view(l) >= value_view(r); }

template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, value>::value || std::is_same<T, value_view>::value) &&
         std::is_convertible<const U &, value_view>::value)
        ||
        ((std::is_same<U, value>::value || std::is_same<U, value_view>::value) &&
         std::is_convertible<const T & , value_view>::value),
        bool>::type
operator >(const T  &l, const U & r) { return value_view(l) >  value_view(r); }




struct key_value_pair
{
    using value_type       = char_type;
    using traits_type      = std::char_traits<char_type>;
    using string_type      = std::basic_string<char_type>;
    using string_view_type = basic_cstring_ref<char_type>;

    key_value_pair() noexcept = default;
    key_value_pair( const key_value_pair& p ) = default;
    key_value_pair( key_value_pair&& p ) noexcept = default;
    key_value_pair(key_view key, value_view value) : value_(key.basic_string<char_type, traits_type>() + equality_sign + 
                                                            value.basic_string<char_type, traits_type>()) {}

    key_value_pair(key_view key, std::initializer_list<basic_string_view<char_type>> values)
    {
        const auto sz = std::accumulate(values.begin(), values.end(),
                                        key.size(), [](std::size_t sz, const auto & str) { return sz + str.size() + 1;});

        value_.reserve(sz);
        value_.append(key.data(), key.size());
        value_ += equality_sign;
        for (auto & value : values)
        {
            if (value_.back() != equality_sign)
                value_ += delimiter;
            value_.append(value.data(), value.size());
        }
    }

    key_value_pair( const string_type& source ) : value_(source) {}
    key_value_pair( string_type&& source ) : value_(std::move(source)) {}
    key_value_pair( const value_type * raw ) : value_(raw) {}
    key_value_pair(       value_type * raw ) : value_(raw) {}

    explicit key_value_pair(key_value_pair_view kv) : value_(kv.c_str()) {}

    template< class Source >
    key_value_pair( const Source& source,
           decltype(source.data()) = nullptr,
           decltype(source.size()) = 0u)
            : value_(boost::process::v2::detail::conv_string<char_type, traits_type>(
                source.data(), source.size()))
    {
    }

    key_value_pair(const typename conditional<is_same<value_type, char>::value, wchar_t, char>::type  * raw)
            : value_(boost::process::v2::detail::conv_string<char_type, traits_type>(
                     raw,
                     std::char_traits<std::decay<std::remove_pointer<decltype(raw)>::type>::type>::length(raw)))
    {
    }
    key_value_pair(const typename conditional<is_same<value_type, char>::value, wchar_t, char>::type  * raw,
                   const std::locale& loc)
            : value_(boost::process::v2::detail::conv_string<char_type, traits_type>(
                     raw,
                     std::char_traits<std::decay<std::remove_pointer<decltype(raw)>::type>::type>::length(raw)))
    {
    }

    template< class InputIt , typename std::iterator_traits<InputIt>::iterator_category>
    key_value_pair( InputIt first, InputIt last )
            : key_value_pair(std::basic_string<typename std::iterator_traits<std::decay_t<InputIt>>::value_type>(first, last))
    {
    }

    ~key_value_pair() = default;

    key_value_pair& operator=( const key_value_pair& p ) = default;
    key_value_pair& operator=( key_value_pair&& p ) noexcept = default;
    key_value_pair& operator=( string_type&& source )
    {
        value_ = std::move(source);
        return *this;
    }
    template< class Source >
    key_value_pair& operator=( const Source& source )
    {
        value_ = boost::process::v2::detail::conv_string<char_type, traits_type>(
            source.data(), source.size());
        return *this;
    }

    key_value_pair& assign( string_type&& source )
    {
        value_ = std::move(source);
        return *this;
    }
    template< class Source >
    key_value_pair& assign( const Source& source )
    {
        value_ = boost::process::v2::detail::conv_string<char_type, traits_type>(
            source.data(), source.size());
        return *this;
    }


    template< class InputIt >
    key_value_pair& assign( InputIt first, InputIt last )
    {
        return assign(std::basic_string<typename std::iterator_traits<std::decay_t<InputIt>>::value_type>(first, last));
    }

    void clear() {value_.clear();}

    void swap( key_value_pair& other ) noexcept
    {
        std::swap(value_, other.value_);
    }

    const value_type* c_str() const noexcept {return value_.c_str();}
    const string_type& native() const noexcept {return value_;}
    string_view_type native_view() const noexcept {return value_;}

    operator string_type() const {return native();}
    operator string_view_type() const {return native_view();}
    operator key_value_pair_view() const {return native_view();}

    int compare( const key_value_pair& p ) const noexcept 
    {
        return key_value_pair_view(*this).compare(key_value_pair_view(p));
    }
    
    int compare( const string_type& str ) const 
    {
        return key_value_pair_view(*this).compare(str);
    }
    int compare( string_view_type str ) const   
    {
        return key_value_pair_view(*this).compare(str);
    }
    int compare( const value_type* s ) const
    {
        return key_value_pair_view(*this).compare(s);
    }

    template< class CharT, class Traits = std::char_traits<CharT>, class Alloc = std::allocator<CharT> >
    std::basic_string<CharT,Traits,Alloc>
    basic_string( const Alloc& alloc = Alloc() ) const
    {
        return boost::process::v2::detail::conv_string<CharT, Traits>(value_.data(), value_.size(), alloc);
    }

    std::string string() const       {return basic_string<char>();}
    std::wstring wstring() const     {return basic_string<wchar_t>();}

    const string_type & native_string() const
    {
        return value_;
    }

    friend bool operator==(const key_value_pair & l, const key_value_pair & r) { return l.compare(r) == 0; }
    friend bool operator!=(const key_value_pair & l, const key_value_pair & r) { return l.compare(r) != 0; }
    friend bool operator<=(const key_value_pair & l, const key_value_pair & r) { return l.compare(r) <= 0; }
    friend bool operator>=(const key_value_pair & l, const key_value_pair & r) { return l.compare(r) >= 0; }
    friend bool operator< (const key_value_pair & l, const key_value_pair & r) { return l.compare(r) <  0; }
    friend bool operator> (const key_value_pair & l, const key_value_pair & r) { return l.compare(r) >  0; }

    bool empty() const {return value_.empty(); }

    struct key_view key() const
    {
        auto eq = value_.find(equality_sign);
        if (eq == 0)
        {
            auto eq2 = value_.find(equality_sign, 1);
            if (eq2 != string_type::npos)
              eq = eq2;
        }
        const auto k = native_view().substr(0, eq);

        return boost::process::v2::environment::key_view::string_view_type (k.data(), k.size());
    }
    struct value_view value() const
    {
        auto eq = value_.find(equality_sign);
        if (eq == 0)
        {
            auto eq2 = value_.find(equality_sign, 1);
            if (eq2 != string_type::npos)
               eq = eq2;
        }
        return value_view::string_view_type(native_view().substr(eq + 1));
    }

    template< class CharT, class Traits >
    friend std::basic_ostream<CharT,Traits>&
    operator<<( std::basic_ostream<CharT,Traits>& os, const key_value_pair& p )
    {
        os << boost::process::v2::quoted(p.basic_string<CharT,Traits>());
        return os;
    }

    template< class CharT, class Traits >
    friend std::basic_istream<CharT,Traits>&
    operator>>( std::basic_istream<CharT,Traits>& is, key_value_pair& p )
    {
        is >> boost::process::v2::quoted(p.value_);
        return is;
    }

    template<std::size_t Idx>
    auto get() const
    {
        if constexpr (Idx == 0u)
            return key_view();
        else
            return value_view();
    }

    template<std::size_t Idx>
    inline auto get() const 
        -> typename conditional<Idx == 0u, boost::process::v2::environment::key_view,
                                           boost::process::v2::environment::value_view>::type;

    const value_type * data() const {return value_.data(); }
    std::size_t size() const {return value_.size(); }

private:
    string_type value_;
};



template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, key_value_pair>::value || std::is_same<T, key_value_pair_view>::value) &&
         std::is_convertible<const U &, key_value_pair_view>::value)
        ||
        ((std::is_same<U, key_value_pair>::value || std::is_same<U, key_value_pair_view>::value) &&
         std::is_convertible<const T & , key_value_pair_view>::value),
        bool>::type
operator==(const T  &l, const U & r) { return key_value_pair_view(l) == key_value_pair_view(r); }

template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, key_value_pair>::value || std::is_same<T, key_value_pair_view>::value) &&
         std::is_convertible<const U &, key_value_pair_view>::value)
        ||
        ((std::is_same<U, key_value_pair>::value || std::is_same<U, key_value_pair_view>::value) &&
         std::is_convertible<const T & , key_value_pair_view>::value),
        bool>::type
operator!=(const T  &l, const U & r) { return key_value_pair_view(l) != key_value_pair_view(r); }

template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, key_value_pair>::value || std::is_same<T, key_value_pair_view>::value) &&
         std::is_convertible<const U &, key_value_pair_view>::value)
        ||
        ((std::is_same<U, key_value_pair>::value || std::is_same<U, key_value_pair_view>::value) &&
         std::is_convertible<const T & , key_value_pair_view>::value),
        bool>::type
operator<=(const T  &l, const U & r) { return key_value_pair_view(l) <= key_value_pair_view(r); }

template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, key_value_pair>::value || std::is_same<T, key_value_pair_view>::value) &&
         std::is_convertible<const U &, key_value_pair_view>::value)
        ||
        ((std::is_same<U, key_value_pair>::value || std::is_same<U, key_value_pair_view>::value) &&
         std::is_convertible<const T & , key_value_pair_view>::value),
        bool>::type
operator <(const T  &l, const U & r) { return key_value_pair_view(l)  < key_value_pair_view(r); }

template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, key_value_pair>::value || std::is_same<T, key_value_pair_view>::value) &&
         std::is_convertible<const U &, key_value_pair_view>::value)
        ||
        ((std::is_same<U, key_value_pair>::value || std::is_same<U, key_value_pair_view>::value) &&
         std::is_convertible<const T & , key_value_pair_view>::value),
        bool>::type
operator>=(const T  &l, const U & r) { return key_value_pair_view(l) >= key_value_pair_view(r); }

template<typename T, typename U>
typename std::enable_if<
        ((std::is_same<T, key_value_pair>::value || std::is_same<T, key_value_pair_view>::value) &&
         std::is_convertible<const U &, key_value_pair_view>::value)
        ||
        ((std::is_same<U, key_value_pair>::value || std::is_same<U, key_value_pair_view>::value) &&
         std::is_convertible<const T & , key_value_pair_view>::value),
        bool>::type
operator >(const T  &l, const U & r) { return key_value_pair_view(l) >  key_value_pair_view(r); }


template<>
inline key_view key_value_pair::get<0u>() const
{
    return key();
}

template<>
inline value_view key_value_pair::get<1u>() const
{
    return value();
}

struct current_view
{
    using native_handle_type = environment::native_handle_type;
    using value_type = key_value_pair_view;

    current_view() = default;
    current_view(current_view && nt) = default;

    native_handle_type  native_handle() { return handle_.get(); }

    struct iterator
    {
        using value_type = key_value_pair_view;
        using iterator_category = std::forward_iterator_tag;

        iterator() = default;
        iterator(const iterator & ) = default;
        iterator(const native_iterator &native_handle) : iterator_(native_handle) {}

        iterator & operator++()
        {
            iterator_ = detail::next(iterator_);
            return *this;
        }

        iterator operator++(int)
        {
            auto last = *this;
            iterator_ = detail::next(iterator_);
            return last;
        }


        const key_value_pair_view operator*() const
        {
            return key_value_pair_view(detail::dereference(iterator_));
        }

        optional<key_value_pair_view> operator->() const
        {
            return key_value_pair_view(detail::dereference(iterator_));
        }

      friend bool operator==(const iterator & l, const iterator & r) {return l.iterator_ == r.iterator_;}
      friend bool operator!=(const iterator & l, const iterator & r) {return l.iterator_ != r.iterator_;}

      private:
        environment::native_iterator iterator_;
    };

    iterator begin() const {return iterator(handle_.get());}
    iterator   end() const {return iterator(detail::find_end(handle_.get()));}

 private:

  std::unique_ptr<typename remove_pointer<native_handle_type>::type,
                    detail::native_handle_deleter> handle_{environment::detail::load_native_handle()};
};

inline current_view current() {return current_view();}

template<typename Environment = current_view>
inline boost::process::v2::filesystem::path home(Environment && env = current())
{
  auto find_key = [&](key_view ky) -> value
  {
    const auto itr = std::find_if(std::begin(env), std::end(env),
                                  [&](key_value_pair vp)
                                  {
                                    auto tmp =  vp.key() == ky;
                                    if (tmp)
                                      return true;
                                    else
                                      return false;
                                  });
    if (itr != nullptr)
      return itr->value_view();
    else
      return value_view();
  };
#if defined(ASIO_WINDOWS)
  return find_key(L"HOMEDRIVE") + find_key(L"HOMEPATH");
#else
  return find_key(L"HOME");
#endif
}

template<typename Environment = current_view>
inline boost::process::v2::filesystem::path find_executable(
                                             boost::process::v2::filesystem::path name,
                                             Environment && env = current())
{
    auto find_key = [&](key_view ky) -> value_view
                    {
                        const auto itr = std::find_if(std::begin(env), std::end(env),
                                            [&](key_value_pair vp)
                                            {
                                                auto tmp =  vp.key() == ky;
                                                if (tmp)
                                                    return true;
                                                else
                                                    return false;
                                            });
                        if (itr != nullptr)
                          return itr->value_view();
                        else
                          return value_view();
                    };

#if defined(BOOST_PROCESS_V2_WINDOWS)
    auto path = find_key(L"PATH");
    auto pathext = find_key(L"PATHEXT");
    for (auto pp_view : path)
        for (auto ext : pathext)
        {
            boost::process::v2::filesystem::path nm(name);
            nm += ext;

            auto p = boost::process::v2::filesystem::path(pp_view) / nm;

            error_code ec;
            bool file = boost::process::v2::filesystem::is_regular_file(p, ec);
            if (!ec && file && SHGetFileInfoW(p.native().c_str(), 0,0,0, SHGFI_EXETYPE))
                return p;
        }
#else
    auto path = find_key("PATH");
    for (auto pp_view : path)
    {
        auto p = boost::process::v2::filesystem::path(pp_view) / name;
        error_code ec;
        bool file = boost::process::v2::filesystem::is_regular_file(p, ec);
        if (!ec && file && ::access(p.c_str(), X_OK) == 0)
            return p;
    }
#endif
    return {};
}


inline value get(const key & k, error_code & ec) { return detail::get(k.c_str(), ec);}
inline value get(const key & k)
{
  error_code ec;
  auto tmp = detail::get(k.c_str(), ec);
  boost::process::v2::detail::throw_error(ec, "environment::get");
  return tmp;
}

inline value get(basic_cstring_ref<char_type, key_char_traits<char_type>> k, error_code & ec)
{
  return detail::get(k, ec);
}
inline value get(basic_cstring_ref<char_type, key_char_traits<char_type>> k)
{
  error_code ec;
  auto tmp = detail::get(k, ec);
  boost::process::v2::detail::throw_error(ec, "environment::get");
  return tmp;
}


inline value get(const char_type * c, error_code & ec) { return detail::get(c, ec);}
inline value get(const char_type * c)
{
  error_code ec;
  auto tmp = detail::get(c, ec);
  boost::process::v2::detail::throw_error(ec, "environment::get");
  return tmp;
}

inline void set(const key & k, value_view vw, error_code & ec) { detail::set(k, vw, ec);}
inline void set(const key & k, value_view vw)
{
  error_code ec;
  detail::set(k, vw, ec);
  boost::process::v2::detail::throw_error(ec, "environment::set");
}

inline void set(basic_cstring_ref<char_type, key_char_traits<char_type>> k, value_view vw, error_code & ec) { detail::set(k, vw, ec);}
inline void set(basic_cstring_ref<char_type, key_char_traits<char_type>> k, value_view vw)
{
  error_code ec;
  detail::set(k, vw, ec);
  boost::process::v2::detail::throw_error(ec, "environment::set");
}


inline void set(const char_type * k, value_view vw, error_code & ec) { detail::set(k, vw, ec);}
inline void set(const char_type * k, value_view vw)
{
  error_code ec;
  detail::set(k, vw, ec);
  boost::process::v2::detail::throw_error(ec, "environment::set");
}

template<typename Char, typename = typename std::enable_if<!std::is_same<Char, char_type>::value>::type>
inline void set(const key & k, const Char * vw, error_code & ec)
{
    value val{vw};
    detail::set(k, val, ec);
}
template<typename Char, typename = typename std::enable_if<!std::is_same<Char, char_type>::value>::type>
inline void set(const key & k, const Char * vw)
{
    error_code ec;
    value val{vw};
    detail::set(k, val, ec);
    boost::process::v2::detail::throw_error(ec, "environment::set");
}

template<typename Char, typename = typename std::enable_if<!std::is_same<Char, char_type>::value>::type>
inline void set(basic_cstring_ref<char_type, key_char_traits<char_type>> k, const Char * vw, error_code & ec)
{
    value val{vw};
    detail::set(k, val, ec);
}
template<typename Char, typename = typename std::enable_if<!std::is_same<Char, char_type>::value>::type>
inline void set(basic_cstring_ref<char_type, key_char_traits<char_type>> k, const Char * vw)
{
    error_code ec;
    value val{vw};
    detail::set(k, val, ec);
    boost::process::v2::detail::throw_error(ec, "environment::set");
}


template<typename Char, typename = typename std::enable_if<!std::is_same<Char, char_type>::value>::type>
inline void set(const char_type * k, const Char * vw, error_code & ec)
{
    value val{vw};
    detail::set(k, val, ec);
}
template<typename Char, typename = typename std::enable_if<!std::is_same<Char, char_type>::value>::type>
inline void set(const char_type * k, const Char * vw)
{
    error_code ec;
    value val{vw};
    detail::set(k, val, ec);
    boost::process::v2::detail::throw_error(ec, "environment::set");
}



inline void unset(const key & k, error_code & ec) { detail::unset(k, ec);}
inline void unset(const key & k)
{
  error_code ec;
  detail::unset(k, ec);
  boost::process::v2::detail::throw_error(ec, "environment::unset");
}

inline void unset(basic_cstring_ref<char_type, key_char_traits<char_type>> k, error_code & ec)
{
  detail::unset(k, ec);
}
inline void unset(basic_cstring_ref<char_type, key_char_traits<char_type>> k)
{
  error_code ec;
  detail::unset(k, ec);
  boost::process::v2::detail::throw_error(ec, "environment::unset");
}


inline void unset(const char_type * c, error_code & ec) { detail::unset(c, ec);}
inline void unset(const char_type * c)
{
  error_code ec;
  detail::unset(c, ec);
  boost::process::v2::detail::throw_error(ec, "environment::unset");
}
}

BOOST_PROCESS_V2_END_NAMESPACE

namespace std
{

template<>
struct tuple_size<boost::process::v2::environment::key_value_pair> : integral_constant<std::size_t, 2u> {};

template<>
struct tuple_element<0u, boost::process::v2::environment::key_value_pair> {using type = boost::process::v2::environment::key_view;};

template<>
struct tuple_element<1u, boost::process::v2::environment::key_value_pair> {using type = boost::process::v2::environment::value_view;};

template<>
struct tuple_size<boost::process::v2::environment::key_value_pair_view> : integral_constant<std::size_t, 2u> {};

template<>
struct tuple_element<0u, boost::process::v2::environment::key_value_pair_view>
{
    using type = boost::process::v2::environment::key_view;
};

template<>
struct tuple_element<1u, boost::process::v2::environment::key_value_pair_view>
{
    using type = boost::process::v2::environment::value_view;
};


}


#endif //BOOST_PROCESS_V2_ENVIRONMENT_HPP
