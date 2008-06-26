#include "Date.hh"

using namespace std;
using namespace boost;

namespace Util {

    time_t ptime_to_time_t(posix_time::ptime t) {
        posix_time::time_duration duration = t - posix_time::from_time_t(0);
        return duration.total_seconds();
    }

    string ptime_to_xmpp_date(posix_time::ptime pt) {
        /* YYYY-MM-DD */
        return gregorian::to_iso_extended_string(pt.date());
    }

    string ptime_to_xmpp_time(posix_time::ptime pt) {
        /* HH:MM:SSZ */
        return posix_time::to_simple_string(pt.time_of_day()) + "Z";
    }

    string ptime_to_xmpp_date_time(posix_time::ptime pt) {
        /* YYYY-MM-DDTHH:MM:SSZ */
        return posix_time::to_iso_extended_string(pt) + "Z";
    }

    posix_time::ptime xmpp_date_time_to_ptime(const string& _date) {
        /* FIXME don't igore time zone */
        string date = _date.substr(0, _date.size()-1);

        /* remove the T separator */
        string::size_type t_pos = date.find('T');
        if(t_pos != string::npos) {
            date[t_pos] = ' ';
        }
        return posix_time::time_from_string(date);
    }

    time_t xmpp_date_time_to_time_t(const string& date) {
        posix_time::ptime pt = xmpp_date_time_to_ptime(date);
        return ptime_to_time_t(pt);
    }

    boost::posix_time::ptime ptime_local_time() {
        return boost::posix_time::microsec_clock::local_time();
    }

    Time ptime_to_systime(boost::posix_time::ptime pt) {
        return Timer::now() + Time::Microseconds((pt - ptime_local_time()).total_microseconds());
    }

    posix_time::ptime systime_to_ptime(Time st) {
        return ptime_local_time() + posix_time::microseconds((st - Timer::now()).getMicroseconds());
    }
}
