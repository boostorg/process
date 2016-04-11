/*
 * string_traits.hpp
 *
 *  Created on: 05.03.2016
 *      Author: Klemens
 */

#ifndef BOOST_PROCESS_DETAIL_TRAITS_CMD_OR_EXE_HPP_
#define BOOST_PROCESS_DETAIL_TRAITS_CMD_OR_EXE_HPP_

#include <string>
#include <vector>
#include <type_traits>
#include <initializer_list>
#include <boost/filesystem/path.hpp>
#include <boost/process/detail/traits/decl.hpp>

namespace boost { namespace process { namespace detail {

struct cmd_or_exe_tag {};

template<> struct initializer_tag_t<boost::filesystem::path> { typedef cmd_or_exe_tag type;};

template<> struct initializer_tag_t<const char*    > { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<const wchar_t* > { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<const char16_t*> { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<const char32_t*> { typedef cmd_or_exe_tag type;};


template<> struct initializer_tag_t<const char*    > { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<const wchar_t* > { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<const char16_t*> { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<const char32_t*> { typedef cmd_or_exe_tag type;};

template<std::size_t Size> struct initializer_tag_t<const char    [Size]> { typedef cmd_or_exe_tag type;};
template<std::size_t Size> struct initializer_tag_t<const wchar_t [Size]> { typedef cmd_or_exe_tag type;};
template<std::size_t Size> struct initializer_tag_t<const char16_t[Size]> { typedef cmd_or_exe_tag type;};
template<std::size_t Size> struct initializer_tag_t<const char32_t[Size]> { typedef cmd_or_exe_tag type;};

template<> struct initializer_tag_t<std::basic_string<char    >> { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<std::basic_string<wchar_t >> { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<std::basic_string<char16_t>> { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<std::basic_string<char32_t>> { typedef cmd_or_exe_tag type;};


template<> struct initializer_tag_t<std::vector<std::basic_string<char    >>> { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<std::vector<std::basic_string<wchar_t >>> { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<std::vector<std::basic_string<char16_t>>> { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<std::vector<std::basic_string<char32_t>>> { typedef cmd_or_exe_tag type;};

template<> struct initializer_tag_t<std::initializer_list<std::basic_string<char    >>> { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<std::initializer_list<std::basic_string<wchar_t >>> { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<std::initializer_list<std::basic_string<char16_t>>> { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<std::initializer_list<std::basic_string<char32_t>>> { typedef cmd_or_exe_tag type;};


template<> struct initializer_tag_t<std::vector<char    *>> { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<std::vector<wchar_t *>> { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<std::vector<char16_t*>> { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<std::vector<char32_t*>> { typedef cmd_or_exe_tag type;};

template<> struct initializer_tag_t<std::initializer_list<char    *>> { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<std::initializer_list<wchar_t *>> { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<std::initializer_list<char16_t*>> { typedef cmd_or_exe_tag type;};
template<> struct initializer_tag_t<std::initializer_list<char32_t*>> { typedef cmd_or_exe_tag type;};

template<>
struct make_initializer_t<cmd_or_exe_tag>;


}}}



#endif /* BOOST_PROCESS_DETAIL_STRING_TRAITS_HPP_ */
