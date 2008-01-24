#ifndef XMLEXCEPTION_HH
#define XMLEXCEPTION_HH

#include <stdexcept>

namespace XML {

    class xml_error : public std::runtime_error {
        public:
            explicit xml_error(const std::string& msg) : runtime_error(msg) { }
    };

    class child_not_found : public xml_error {
        public:
            explicit child_not_found(const std::string& msg) : xml_error(msg) { }
    };

    class attribute_not_found : public xml_error {
        public:
            explicit attribute_not_found(const std::string& msg) : xml_error(msg) { }
    };

}
#endif
