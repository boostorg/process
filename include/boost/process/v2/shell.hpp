// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_SHELL_HPP
#define BOOST_PROCESS_V2_SHELL_HPP

#include <boost/core/exchange.hpp>
#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/throw_error.hpp>
#include <boost/process/v2/cstring_ref.hpp>
#include <memory>

BOOST_PROCESS_V2_BEGIN_NAMESPACE


extern BOOST_PROCESS_V2_DECL const error_category& get_shell_category();
static const error_category& shell_category = get_shell_category();

struct shell
{
    
#if defined(BOOST_PROCESS_V2_WINDOWS)
    using char_type = wchar_t; 
#else
    using char_type = char;
#endif

    template<typename Char, typename Traits>
    shell(basic_cstring_ref<Char, Traits> input) 
        : buffer_(conv_string(input.data(), input.size())) 
    {
    }

    shell(basic_cstring_ref<char_type> input) : input_(input) {}

    struct argv_t
    {
        using char_type = shell::char_type;

        int argc() const { return argc_; }
        char_type** argv() const { return argv_; }
        
        char_type** begin() const {return argv();}
        char_type** end()   const {return argv() + argc();}

        bool empty() const {return argc() == 0;}

        argv_t() = default;
        argv_t(basic_cstring_ref<char_type> input)
        {
            error_code ec;
            *this = parse_(input, ec);
            if (ec)
                detail::throw_error(ec, "parse-argv");
        }
        argv_t(const argv_t &) = delete;
        argv_t& operator=(const argv_t &) = delete;

        argv_t(argv_t && lhs) noexcept 
            : argc_(boost::exchange(lhs.argc_, 0)),
              argv_(boost::exchange(lhs.argv_, nullptr)),
              reserved_(boost::exchange(lhs.reserved_, 0))  
        {
        }
        argv_t& operator=(argv_t && lhs) noexcept
        {
            argv_t tmp(std::move(*this));
            argc_  = boost::exchange(lhs.argc_, 0);
            argv_ = boost::exchange(lhs.argv_, nullptr);
            reserved_ = boost::exchange(lhs.reserved_, 0);
            return *this;
        }
        BOOST_PROCESS_V2_DECL ~argv_t();
      private:
        BOOST_PROCESS_V2_DECL static 
        argv_t parse_(basic_cstring_ref<char_type> input, error_code & ec);
        friend struct shell;
        int argc_ = 0;
        char_type  ** argv_ = nullptr;
        int reserved_ = 0;
    };

    argv_t parse() const
    {
        error_code ec;
        auto tmp = parse(ec);
        if (ec)
            detail::throw_error(ec, "parse cmd_line");
        return tmp;
    }

    argv_t parse(error_code & ec) const noexcept
    {
        return argv_t::parse_(cmd_line(), ec);
    }

    basic_cstring_ref<char_type> cmd_line() const noexcept
    {
        return buffer_.empty() ? input_ : buffer_.c_str();
    }

  private:
    // storage in case we need a conversion
    std::basic_string<char_type> buffer_;
    basic_cstring_ref<char_type> input_;           
};

BOOST_PROCESS_V2_END_NAMESPACE

#if defined(BOOST_PROCESS_V2_HEADER_ONLY)

#include <boost/process/v2/impl/shell.ipp>

#endif

#endif //BOOST_PROCESS_V2_ERROR_HPP
