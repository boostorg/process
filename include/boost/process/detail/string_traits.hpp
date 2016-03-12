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

namespace boost { namespace process {

namespace detail {

template<typename T>
constexpr inline std::false_type is_unique   (const T & ) {return {}; }
template<typename T>
constexpr inline std::false_type is_sequence (const T & ) {return {}; }

constexpr inline std::true_type is_sequence  (const char*)     {return {};}
constexpr inline std::true_type is_sequence  (const wchar_t*)  {return {};}
constexpr inline std::true_type is_sequence  (const char16_t*) {return {};}
constexpr inline std::true_type is_sequence  (const char32_t*) {return {};}


constexpr inline std::true_type is_sequence  (const std::basic_string<char>&)     {return {};}
constexpr inline std::true_type is_sequence  (const std::basic_string<wchar_t>&)  {return {};}
constexpr inline std::true_type is_sequence  (const std::basic_string<char16_t>&) {return {};}
constexpr inline std::true_type is_sequence  (const std::basic_string<char32_t>&) {return {};}

constexpr inline std::true_type is_sequence  (const std::vector<std::basic_string<char>>     &) {return {};}
constexpr inline std::true_type is_sequence  (const std::vector<std::basic_string<wchar_t>>  &) {return {};}
constexpr inline std::true_type is_sequence  (const std::vector<std::basic_string<char16_t>> &) {return {};}
constexpr inline std::true_type is_sequence  (const std::vector<std::basic_string<char32_t>> &) {return {};}

template<typename T>
constexpr inline std::false_type is_string (const T & ) {return {}; }

constexpr inline std::true_type is_string  (const char*)     {return {};}
constexpr inline std::true_type is_string  (const wchar_t*)  {return {};}
constexpr inline std::true_type is_string  (const char16_t*) {return {};}
constexpr inline std::true_type is_string  (const char32_t*) {return {};}


constexpr inline std::true_type is_string  (const std::basic_string<char>&)     {return {};}
constexpr inline std::true_type is_string  (const std::basic_string<wchar_t>&)  {return {};}
constexpr inline std::true_type is_string  (const std::basic_string<char16_t>&) {return {};}
constexpr inline std::true_type is_string  (const std::basic_string<char32_t>&) {return {};}


template<typename T>
constexpr inline std::false_type is_string_list (const T & ) {return {}; }

constexpr inline std::true_type is_string_list  (const std::vector<std::basic_string<char>>     &) {return {};}
constexpr inline std::true_type is_string_list  (const std::vector<std::basic_string<wchar_t>>  &) {return {};}
constexpr inline std::true_type is_string_list  (const std::vector<std::basic_string<char16_t>> &) {return {};}
constexpr inline std::true_type is_string_list  (const std::vector<std::basic_string<char32_t>> &) {return {};}


template<typename T>
struct is_string_t : decltype(is_string(T())) {};

template<typename T>
struct is_string_list_t : decltype(is_string_list(T())) {};

}}}



#endif /* BOOST_PROCESS_DETAIL_STRING_TRAITS_HPP_ */
