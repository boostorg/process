/*
 * no_wait_io_service.hpp
 *
 *  Created on: 12.06.2016
 *      Author: Klemens
 */

#ifndef BOOST_PROCESS_DETAIL_NO_WAIT_IO_SERVICE_HPP_
#define BOOST_PROCESS_DETAIL_NO_WAIT_IO_SERVICE_HPP_


#include <boost/process/detail/config.hpp>

#if defined(BOOST_POSIX_API)
#include <boost/process/detail/posix/handler.hpp>
#include <boost/process/detail/posix/asio_fwd.hpp>
#else
#include <boost/process/detail/windows/handler.hpp>
#include <boost/process/detail/windows/asio_fwd.hpp>
#endif

namespace boost { namespace process { namespace detail {

struct no_wait_io_service_ref : api::handler_base_ext
{
    boost::asio::io_service &get() {return ios;};
    no_wait_io_service_ref(boost::asio::io_service & ios)
            : ios(ios)
    {
    }
private:
    boost::asio::io_service &ios;
};

// mimes std::forward

template<typename T>
inline T&& make_no_wait_io_service_ref(typename std::remove_reference<T>::type& v)
{
    return static_cast<T&&>(v);
}

template<typename T>
inline T&& make_no_wait_io_service_ref(typename std::remove_reference<T>::type&& v) noexcept
{
    return static_cast<T&&>(v);
}

inline no_wait_io_service_ref
           make_no_wait_io_service_ref(boost::asio::io_service & ios)
{
    return ios;
}

inline no_wait_io_service_ref
           make_no_wait_io_service_ref(boost::asio::io_service && ios)
{
    return ios;
}


}
}
}




#endif /* BOOST_PROCESS_DETAIL_NO_WAIT_IO_SERVICE_HPP_ */
