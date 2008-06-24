#ifndef UTIL_DATE_HH
#define UTIL_DATE_HH

#include <boost/date_time/posix_time/posix_time.hpp>

#include <string>

namespace Util {

    time_t ptime_to_time_t(boost::posix_time::ptime t);

    std::string ptime_to_xmpp_date(boost::posix_time::ptime pt);

    std::string ptime_to_xmpp_time(boost::posix_time::ptime pt);

    std::string ptime_to_xmpp_date_time(boost::posix_time::ptime pt);

    time_t xmpp_date_time_to_time_t(const std::string& _date);
}

#endif
