#ifndef GAMEEXCEPTIONS_HH
#define GAMEEXCEPTIONS_HH

#include <stdexcept>

class game_exception : public std::runtime_error {
	public:
		game_exception(const std::string &msg) : runtime_error(msg) { }
};

class missing_information : public game_exception {
	public:
		missing_information(const std::string &msg) : runtime_error(msg) { }
};

class bad_information : public game_exception {
	public:
		bad_information(const std::string &msg) : runtime_error(msg) { }
};

class wrong_turn : public game_exception {
	public:
		wrong_turn(const std::string &msg) : game_exception(msg) { }
};

class invalid_move : public game_exception {
	public:
		invalid_move(const std::string &msg) : game_exception(msg) { }
};

#endif
