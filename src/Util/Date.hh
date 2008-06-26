#ifndef UTIL_DATE_HH
#define UTIL_DATE_HH

#include <boost/date_time/posix_time/posix_time.hpp>

#include <string>

#include "Util/Timer.hh"

namespace Util {

    time_t ptime_to_time_t(boost::posix_time::ptime t);

    std::string ptime_to_xmpp_date(boost::posix_time::ptime pt);

    std::string ptime_to_xmpp_time(boost::posix_time::ptime pt);

    std::string ptime_to_xmpp_date_time(boost::posix_time::ptime pt);

    time_t xmpp_date_time_to_time_t(const std::string& date);

    boost::posix_time::ptime xmpp_date_time_to_ptime(
            const std::string& xmpp_date_time);

    boost::posix_time::ptime ptime_local_time();

    Time ptime_to_systime(boost::posix_time::ptime pt);

    boost::posix_time::ptime systime_to_ptime(Time pt);

}

#endif
