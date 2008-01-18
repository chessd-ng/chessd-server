#ifndef EXCPETIONS_HH
#define EXCPETIONS_HH

#include <exception>
#include <stdexcept>

class user_error : public std::runtime_error
{
    public:
        user_error(const std::string& msg) : std::runtime_error(msg) { }
};

#endif
