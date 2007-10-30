#ifndef EXCEPTIONS_HH
#define EXCEPTIONS_HH

#include <exception>

class KeyError : public std::exception {
	public:
		KeyError(const std::string& key) : msg(msg) { }
		virtual ~KeyError() throw() { }
		virtual const char* what() const throw() {
			return ("KeyError: " + this->msg).c_str();
		}
	private:
		std::string msg;
};

class ValueError : public std::exception {
	public:
		ValueError(const std::string& msg) : msg(msg) { }
		virtual ~ValueError() throw() { }
		virtual const char* what() const throw() {
			return ("ValueError: " + msg).c_str();
		}
	private:
		std::string msg;
};

class IOError : public std::exception {
	public:
		IOError(const std::string& msg) : msg(msg) { }
		virtual ~ValueError() throw() { }
		virtual const char* what() const throw() {
			return ("IOError: " + msg).c_str();
		}
	private:
		std::string msg;
};

#endif
