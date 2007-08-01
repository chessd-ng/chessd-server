#ifndef GAME_MOVE_HH
#define GAME_MOVE_HH

#include <string>

class GameMove {
	public:
		GameMove() { }
		GameMove(const std::string& code, int time) :
			_code(code), _time(time) { }

		const std::string& code() const { return this->_code; }
		std::string& code() { return this->_code; }

		const int& time() const { return this->_time; }
		int& time() { return this->_time; }

	private:
		std::string _code;
		int _time;
};

#endif
