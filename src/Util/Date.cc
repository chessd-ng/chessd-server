#include "Date.hh"

namespace Util {

    time_t ptime_to_time_t(boost::posix_time::ptime t) {
        boost::posix_time::time_duration duration = t - boost::posix_time::from_time_t(0);
        return duration.total_seconds();
    }

    std::string ptime_to_xmpp_date(boost::posix_time::ptime pt) {
        /* YYYY-MM-DD */
        return boost::gregorian::to_iso_extended_string(pt.date());
    }

    std::string ptime_to_xmpp_time(boost::posix_time::ptime pt) {
        /* HH:MM:SSZ */
        return boost::posix_time::to_simple_string(pt.time_of_day()) + "Z";
    }

    std::string ptime_to_xmpp_date_time(boost::posix_time::ptime pt) {
        /* YYYY-MM-DDTHH:MM:SSZ */
        return boost::posix_time::to_iso_extended_string(pt) + "Z";
    }

    int xmpp_date_time_to_time_t(const std::string& _date) {
        /* FIXME don't igore time zone */
        std::string date = _date.substr(0, _date.size()-1);

        /* remove the T separator */
        std::string::size_type t_pos = date.find('T');
        if(t_pos != std::string::npos) {
            date[t_pos] = ' ';
        }
        boost::posix_time::ptime pt = boost::posix_time::time_from_string(date);
        return ptime_to_time_t(pt);
    }

}
