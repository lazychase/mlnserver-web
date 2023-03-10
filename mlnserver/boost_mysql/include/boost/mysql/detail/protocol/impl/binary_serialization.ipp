//
// Copyright (c) 2019-2022 Ruben Perez Hidalgo (rubenperez038 at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MYSQL_DETAIL_PROTOCOL_IMPL_BINARY_SERIALIZATION_IPP
#define BOOST_MYSQL_DETAIL_PROTOCOL_IMPL_BINARY_SERIALIZATION_IPP

#pragma once

#include <boost/mysql/detail/protocol/binary_serialization.hpp>
#include <boost/mysql/detail/protocol/constants.hpp>
#include <boost/mysql/detail/protocol/date.hpp>

namespace boost {
namespace mysql {
namespace detail {

// Floating point types
template <class T>
void serialize_binary_float(serialization_context& ctx, T input)
{
    boost::endian::endian_store<T, sizeof(T), boost::endian::order::little>(ctx.first(), input);
    ctx.advance(sizeof(T));
}

// Time types

// Does not add the length prefix byte
inline void serialize_binary_ymd(serialization_context& ctx, const year_month_day& ymd) noexcept
{
    assert(ymd.years >= 0 && ymd.years <= 0xffff);
    serialize(
        ctx,
        static_cast<std::uint16_t>(ymd.years),
        static_cast<std::uint8_t>(ymd.month),
        static_cast<std::uint8_t>(ymd.day)
    );
}

inline void serialize_binary_date(serialization_context& ctx, const date& input)
{
    assert(input >= min_date && input <= max_date);
    auto ymd = days_to_ymd(input.time_since_epoch().count());

    serialize(ctx, static_cast<std::uint8_t>(binc::date_sz));
    serialize_binary_ymd(ctx, ymd);
}

inline void serialize_binary_datetime(serialization_context& ctx, const datetime& input)
{
    assert(input >= min_datetime && input <= max_datetime);

    // Break datetime
    using namespace std::chrono;
    auto input_dur = input.time_since_epoch();
    auto num_micros = duration_cast<microseconds>(input_dur % seconds(1));
    auto num_secs = duration_cast<seconds>(input_dur % minutes(1) - num_micros);
    auto num_mins = duration_cast<minutes>(input_dur % hours(1) - num_secs);
    auto num_hours = duration_cast<hours>(input_dur % days(1) - num_mins);
    auto num_days = duration_cast<days>(input_dur - num_hours);

    // Serialize
    serialize(ctx, static_cast<std::uint8_t>(binc::datetime_dhmsu_sz));
    serialize_binary_ymd(ctx, days_to_ymd(num_days.count()));
    serialize(
        ctx,
        static_cast<std::uint8_t>(num_hours.count()),
        static_cast<std::uint8_t>(num_mins.count()),
        static_cast<std::uint8_t>(num_secs.count()),
        static_cast<std::uint32_t>(num_micros.count())
    );
}

inline void serialize_binary_time(serialization_context& ctx, const time& input)
{
    // Break time
    using namespace std::chrono;
    auto num_micros = duration_cast<microseconds>(input % seconds(1));
    auto num_secs = duration_cast<seconds>(input % minutes(1) - num_micros);
    auto num_mins = duration_cast<minutes>(input % hours(1) - num_secs);
    auto num_hours = duration_cast<hours>(input % days(1) - num_mins);
    auto num_days = duration_cast<days>(input - num_hours);
    std::uint8_t is_negative = (input.count() < 0) ? 1 : 0;

    // Serialize
    serialize(
        ctx,
        static_cast<std::uint8_t>(binc::time_dhmsu_sz),
        is_negative,
        static_cast<std::uint32_t>(std::abs(num_days.count())),
        static_cast<std::uint8_t>(std::abs(num_hours.count())),
        static_cast<std::uint8_t>(std::abs(num_mins.count())),
        static_cast<std::uint8_t>(std::abs(num_secs.count())),
        static_cast<std::uint32_t>(std::abs(num_micros.count()))
    );
}

}  // namespace detail
}  // namespace mysql
}  // namespace boost

inline std::size_t boost::mysql::detail::
    serialization_traits<boost::mysql::field_view, boost::mysql::detail::serialization_tag::none>::
        get_size_(const serialization_context& ctx, const field_view& input) noexcept
{
    switch (input.kind())
    {
    case field_kind::null: return 0;
    case field_kind::int64: return 8;
    case field_kind::uint64: return 8;
    case field_kind::string: return get_size(ctx, string_lenenc(input.get_string()));
    case field_kind::float_: return 4;
    case field_kind::double_: return 8;
    case field_kind::date: return binc::date_sz + binc::length_sz;
    case field_kind::datetime: return binc::datetime_dhmsu_sz + binc::length_sz;
    case field_kind::time: return binc::time_dhmsu_sz + binc::length_sz;
    default: assert(false); return 0;
    }
}

inline void boost::mysql::detail::
    serialization_traits<boost::mysql::field_view, boost::mysql::detail::serialization_tag::none>::
        serialize_(serialization_context& ctx, const field_view& input) noexcept
{
    switch (input.kind())
    {
    case field_kind::null: break;
    case field_kind::int64: serialize(ctx, input.get_int64()); break;
    case field_kind::uint64: serialize(ctx, input.get_uint64()); break;
    case field_kind::string: serialize(ctx, string_lenenc(input.get_string())); break;
    case field_kind::float_: serialize_binary_float(ctx, input.get_float()); break;
    case field_kind::double_: serialize_binary_float(ctx, input.get_double()); break;
    case field_kind::date: serialize_binary_date(ctx, input.get_date()); break;
    case field_kind::datetime: serialize_binary_datetime(ctx, input.get_datetime()); break;
    case field_kind::time: serialize_binary_time(ctx, input.get_time()); break;
    default: assert(false); break;
    }
}

#endif
