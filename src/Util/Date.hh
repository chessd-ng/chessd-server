#ifndef UTIL_DATE_HH
#define UTIL_DATE_HH

#include <boost/date_time/posix_time/posix_time.hpp>

#include <string>

#include <ctime>

namespace Util {

    inline time_t ptime_to_time_t(boost::posix_time::ptime t) {
        boost::posix_time::time_duration duration = t - boost::posix_time::from_time_t(0);
        return duration.total_seconds();
    }

    inline std::string ptime_to_xmpp_date(boost::posix_time::ptime pt) {
        char str[1024];
        tm t = boost::posix_time::to_tm(pt);
        strftime(str, sizeof(str), "%Y-%m-%d", &t);
        return str;
    }

    inline std::string ptime_to_xmpp_time(boost::posix_time::ptime pt) {
        char str[1024];
        tm t = boost::posix_time::to_tm(pt);
        strftime(str, sizeof(str), "%H:%M:%SZ", &t);
        return str;
    }

    inline std::string ptime_to_xmpp_date_time(boost::posix_time::ptime pt) {
        char str[1024];
        tm t = boost::posix_time::to_tm(pt);
        strftime(str, sizeof(str), "%Y-%m-%dT%H:%M:%SZ", &t);
        return str;
    }

}

#endif
