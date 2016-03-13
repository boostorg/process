/*
 * string_traits.hpp
 *
 *  Created on: 05.03.2016
 *      Author: Klemens
 */

#ifndef BOOST_PROCESS_DETAIL_STRING_TRAITS_HPP_
#define BOOST_PROCESS_DETAIL_STRING_TRAITS_HPP_

#include <string>
#include <type_traits>
#include <vector>
#include <initializer_list>
#include <boost/filesystem/path.hpp>

namespace boost { namespace process {

namespace detail {

struct cmd_or_exe_tag {};

constexpr inline cmd_or_exe_tag initializer_tag  (const boost::filesystem::path &)     {return {};}


constexpr inline cmd_or_exe_tag initializer_tag  (const char*)     {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const wchar_t*)  {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const char16_t*) {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const char32_t*) {return {};}

template<std::size_t Size>
constexpr inline cmd_or_exe_tag initializer_tag  (const char    (&)[Size]) {return {};}
template<std::size_t Size>
constexpr inline cmd_or_exe_tag initializer_tag  (const wchar_t (&)[Size]) {return {};}
template<std::size_t Size>
constexpr inline cmd_or_exe_tag initializer_tag  (const char16_t(&)[Size]) {return {};}
template<std::size_t Size>
constexpr inline cmd_or_exe_tag initializer_tag  (const char32_t(&)[Size]) {return {};}



constexpr inline cmd_or_exe_tag initializer_tag  (const std::basic_string<char>&)     {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const std::basic_string<wchar_t>&)  {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const std::basic_string<char16_t>&) {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const std::basic_string<char32_t>&) {return {};}

constexpr inline cmd_or_exe_tag initializer_tag  (const std::vector<std::basic_string<char>>     &) {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const std::vector<std::basic_string<wchar_t>>  &) {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const std::vector<std::basic_string<char16_t>> &) {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const std::vector<std::basic_string<char32_t>> &) {return {};}

constexpr inline cmd_or_exe_tag initializer_tag  (const std::initializer_list<std::basic_string<char>>     &) {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const std::initializer_list<std::basic_string<wchar_t>>  &) {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const std::initializer_list<std::basic_string<char16_t>> &) {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const std::initializer_list<std::basic_string<char32_t>> &) {return {};}


constexpr inline cmd_or_exe_tag initializer_tag  (const std::vector<const char *>     &) {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const std::vector<const wchar_t*>  &) {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const std::vector<const char16_t*> &) {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const std::vector<const char32_t*> &) {return {};}

constexpr inline cmd_or_exe_tag initializer_tag  (const std::initializer_list<const char*>     &) {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const std::initializer_list<const wchar_t*>  &) {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const std::initializer_list<const char16_t*> &) {return {};}
constexpr inline cmd_or_exe_tag initializer_tag  (const std::initializer_list<const char32_t*> &) {return {};}


}}}



#endif /* BOOST_PROCESS_DETAIL_STRING_TRAITS_HPP_ */
