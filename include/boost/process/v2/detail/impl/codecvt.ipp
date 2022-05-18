// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_DETAIL_IMPL_CODECVT_IPP
#define BOOST_PROCESS_V2_DETAIL_IMPL_CODECVT_IPP

#if defined(BOOST_PROCESS_V2_WINDOWS)

#include <boost/process/v2/detail/config.hpp>
#include <boost/process/v2/detail/codecvt.hpp>

#include <windows.h>

BOOST_PROCESS_V2_BEGIN_NAMESPACE
namespace detail {


//copied from boost.filesystem
class windows_codecvt
   : public std::codecvt< wchar_t, char, std::mbstate_t >
{
 public:
  explicit windows_codecvt(std::size_t ref_count = 0)
       : std::codecvt<wchar_t, char, std::mbstate_t>(ref_count) {}
 protected:

  bool do_always_noconv() const noexcept override { return false; }

  //  seems safest to assume variable number of characters since we don't
  //  actually know what codepage is active
  int do_encoding() const noexcept override { return 0; }

  std::codecvt_base::result do_in(std::mbstate_t& state,
     const char* from, const char* from_end, const char*& from_next,
     wchar_t* to, wchar_t* to_end, wchar_t*& to_next) const override
  {

    auto codepage =
#if !defined(BOOST_NO_ANSI_APIS)
               ::AreFileApisANSI() ? CP_ACP :
#endif
               CP_OEMCP;

    result res = ok;
    int count = 0;
    if ((count = ::MultiByteToWideChar(codepage, MB_PRECOMPOSED,
                                        from, static_cast<int>(from_end - from),
                                        to, static_cast<int>(to_end - to))) == 0)
    {
       switch (::GetLastError())
       {
         case ERROR_INSUFFICIENT_BUFFER:
           // A supplied buffer size was not large enough, or it was incorrectly set to NULL.
           res = partial;
           break;
         case ERROR_INVALID_FLAGS:
           // The values supplied for flags were not valid.
           res = error;
           break;
         case ERROR_INVALID_PARAMETER:
           // Any of the parameter values was invalid.
           res = error;
           break;
         case ERROR_NO_UNICODE_TRANSLATION:
           // Invalid Unicode was found in a string.
           res = error;
           break;
      }
    }
    if (res != error)
    {
      from_next = from_end;
      to_next = to + count;
      *to_next = L'\0';
    }
    return res;
  }

  std::codecvt_base::result do_out(std::mbstate_t & state,
     const wchar_t* from, const wchar_t* from_end, const wchar_t*& from_next,
     char* to, char* to_end, char*& to_next) const override
  {
    auto codepage =
#if !defined(BOOST_NO_ANSI_APIS)
                   ::AreFileApisANSI() ? CP_ACP :
#endif
                     CP_OEMCP;
    result res = ok;

    int count = 0;


    if ((count = WideCharToMultiByte(codepage,
                 WC_NO_BEST_FIT_CHARS, from,
                 static_cast<int>(from_end - from), to, static_cast<int>(to_end - to), 0, 0)) == 0)
    {
      switch (::GetLastError())
      {
        case ERROR_INSUFFICIENT_BUFFER:
          // A supplied buffer size was not large enough, or it was incorrectly set to NULL.
          res = partial;
          break;
        case ERROR_INVALID_FLAGS:
          // The values supplied for flags were not valid.
          res = error;
          break;
        case ERROR_INVALID_PARAMETER:
          // Any of the parameter values was invalid.
          res = error;
          break;
        case ERROR_NO_UNICODE_TRANSLATION:
          // Invalid Unicode was found in a string.
          res = error;
          break;
      }    }
    if (res != error)
    {
      from_next = from_end;
      to_next = to + count;
      *to_next = '\0';
    }
    return res;
  }

  std::codecvt_base::result do_unshift(std::mbstate_t&,
       char* /*from*/, char* /*to*/, char* & /*next*/) const override { return ok; }

  int do_length(std::mbstate_t&,
               const char* from, const char* from_end, std::size_t /*max*/) const override
  {
    return std::distance(from, from_end);
  }

   int do_max_length() const noexcept override { return 0; }
 };

BOOST_PROCESS_V2_DECL const std::codecvt< wchar_t, char, std::mbstate_t > & default_codecvt()
{
  const static const windows_codecvt cvt{1};
  return cvt;
}

}
BOOST_PROCESS_V2_END_NAMESPACE

#endif

#endif //BOOST_PROCESS_V2_DETAIL_IMPL_CODECVT_IPP
