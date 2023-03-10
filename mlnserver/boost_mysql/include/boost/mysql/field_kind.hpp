//
// Copyright (c) 2019-2022 Ruben Perez Hidalgo (rubenperez038 at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MYSQL_FIELD_KIND_HPP
#define BOOST_MYSQL_FIELD_KIND_HPP

#include <iosfwd>

namespace boost {
namespace mysql {

/**
 * \brief Represents the possible C++ types a [reflink field] or [reflink field_view] may have.
 */
enum class field_kind
{
    // Order here is important

    /// Any of the below when the value is NULL
    null = 0,

    /// The field contains a `std::int64_t`.
    int64,

    /// The field contains a `std::uint64_t`.
    uint64,

    /// The field contains a string (`std::string` for `field` and `boost::string_view` for
    /// `field_view`).
    string,

    /// The field contains a `float`.
    float_,

    /// The field contains a `double`.
    double_,

    /// The field contains a [reflink date].
    date,

    /// The field contains a [reflink datetime].
    datetime,

    /// The field contains a [reflink time].
    time
};

/**
 * \relates field_kind
 * \brief Streams a field_kind.
 */
inline std::ostream& operator<<(std::ostream& os, field_kind v);

}  // namespace mysql
}  // namespace boost

#include <boost/mysql/impl/field_kind.ipp>

#endif
