#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>

#include "Log.hh"

using namespace std;
using namespace Util;

Log Util::log;

Log::Log() : output(&cout) { }

void Log::setOutput(ostream& output) {
    this->output = &output;
}

void Log::log(const std::string& msg) {
    *(this->output) << boost::posix_time::second_clock::local_time() << ": " << msg << endl;
}
