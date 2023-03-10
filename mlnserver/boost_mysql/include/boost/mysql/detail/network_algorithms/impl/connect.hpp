//
// Copyright (c) 2019-2022 Ruben Perez Hidalgo (rubenperez038 at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MYSQL_DETAIL_NETWORK_ALGORITHMS_IMPL_CONNECT_HPP
#define BOOST_MYSQL_DETAIL_NETWORK_ALGORITHMS_IMPL_CONNECT_HPP

#pragma once

#include <boost/mysql/detail/network_algorithms/connect.hpp>
#include <boost/mysql/detail/network_algorithms/handshake.hpp>

namespace boost {
namespace mysql {
namespace detail {

template <class Stream>
struct connect_op : boost::asio::coroutine
{
    using endpoint_type = typename Stream::lowest_layer_type::endpoint_type;

    channel<Stream>& chan_;
    error_info& output_info_;
    endpoint_type ep_;
    handshake_params params_;

    connect_op(
        channel<Stream>& chan,
        error_info& output_info,
        const endpoint_type& ep,
        const handshake_params& params
    )
        : chan_(chan), output_info_(output_info), ep_(ep), params_(params)
    {
    }

    template <class Self>
    void operator()(Self& self, error_code code = {})
    {
        BOOST_ASIO_CORO_REENTER(*this)
        {
            output_info_.clear();

            // Physical connect
            BOOST_ASIO_CORO_YIELD chan_.lowest_layer().async_connect(ep_, std::move(self));
            if (code)
            {
                chan_.close();
                output_info_.set_message("Physical connect failed");
                self.complete(code);
                BOOST_ASIO_CORO_YIELD break;
            }

            // Handshake
            BOOST_ASIO_CORO_YIELD async_handshake(chan_, params_, output_info_, std::move(self));
            if (code)
            {
                chan_.close();
            }
            self.complete(code);
        }
    }
};

}  // namespace detail
}  // namespace mysql
}  // namespace boost

template <class Stream>
void boost::mysql::detail::connect(
    channel<Stream>& chan,
    const typename Stream::lowest_layer_type::endpoint_type& endpoint,
    const handshake_params& params,
    error_code& err,
    error_info& info
)
{
    chan.lowest_layer().connect(endpoint, err);
    if (err)
    {
        chan.close();
        info.set_message("Physical connect failed");
        return;
    }
    handshake(chan, params, err, info);
    if (err)
    {
        chan.close();
    }
}

template <class Stream, class CompletionToken>
BOOST_ASIO_INITFN_AUTO_RESULT_TYPE(CompletionToken, void(boost::mysql::error_code))
boost::mysql::detail::async_connect(
    channel<Stream>& chan,
    const typename Stream::lowest_layer_type::endpoint_type& endpoint,
    const handshake_params& params,
    error_info& info,
    CompletionToken&& token
)
{
    return boost::asio::async_compose<CompletionToken, void(error_code)>(
        connect_op<Stream>{chan, info, endpoint, params},
        token,
        chan
    );
}

#endif /* INCLUDE_BOOST_MYSQL_DETAIL_NETWORK_ALGORITHMS_IMPL_CONNECT_HPP_ */
