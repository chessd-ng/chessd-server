#ifndef LOG_HH
#define LOG_HH

#include <ostream>
#include <string>

namespace Util {
    class Log {
        public:

            Log();

            void setOutput(std::ostream& output);

            void log(const std::string& msg);
        private:

            std::ostream* output;
    };

    extern Log log;
}


#endif
