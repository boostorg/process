// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_IMPL_SHELL_IPP
#define BOOST_PROCESS_V2_IMPL_SHELL_IPP

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/throw_error.hpp>
#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/error.hpp>
#include <boost/process/v2/shell.hpp>

#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <shellapi.h>
#else
#include <wordexp.h>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

#if defined(BOOST_PROCESS_V2_WINDOWS)
BOOST_PROCESS_V2_DECL const error_category& get_shell_category()
{
    return system_category();
}
#else

struct shell_category_t final : public error_category
{
    shell_category_t() : error_category(0xDAF1u) {}

    const char* name() const noexcept
    {
        return "process.v2.utf8";
    }
    std::string message(int value) const
    {
        switch (value)
        {
        case WRDE_BADCHAR:
            return "Illegal occurrence of newline or one of |, &, ;, <, >, (, ), {, }.";
        case WRDE_BADVAL:
            return "An undefined shell variable was referenced, and the WRDE_UNDEF flag told us to consider this an error.";
        case WRDE_CMDSUB:
            return "Command substitution occurred, and the WRDE_NOCMD flag told us to consider this an error.";
        case WRDE_NOSPACE:
            return "Out of memory.";
        case WRDE_SYNTAX:
            return "Shell syntax error, such as unbalanced parentheses or unmatched quotes.";
        default:
            return "process.v2.wordexp error";
        }
    }
};

BOOST_PROCESS_V2_DECL const error_category& get_shell_category()
{
    static shell_category_t instance;
    return instance;
}

#endif

shell::argv_t shell::argv_t::parse_(
    basic_cstring_ref<shell::char_type> input, 
    error_code & ec)
{
    shell::argv_t res;
    wordexp_t we{};
    auto cd = wordexp(input.c_str(), &we, WRDE_NOCMD);

    if (cd != 0)
        ec.assign(cd, get_shell_category());
    else
    {
        res.argc_ = static_cast<int>(we.we_wordc);
        res.argv_ = we.we_wordv;
        res.reserved_ = static_cast<int>(we.we_offs); 
    }

    return res;
}

shell::argv_t::~argv_t()
{
    if (argv_ != nullptr)
    {
        wordexp_t we{
            .we_wordc = static_cast<std::size_t>(argc_),
            .we_wordv = argv_,
            .we_offs = static_cast<std::size_t>(reserved_)
        };
        wordfree(&we);
    }
}

BOOST_PROCESS_V2_END_NAMESPACE

#endif //BOOST_PROCESS_V2_IMPL_SHELL_IPP
