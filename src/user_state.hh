#ifndef USER_STATE_HH
#define USER_STATE_HH

class UserState {
	public:
		UserState(bool accepting, bool playing) :
			_accept_match(accepting), _playing(playing) { }

		const bool& isAcceptingMatch() const { return this->_accept_match; }
		bool& isAcceptingMatch() { return this->_accept_match; }

		const bool& isPlaying() const { return this->_playing; }
		bool& isPlaying() { return this->_playing; }

	private:
		bool _accept_match;
		bool _playing;
};

#endif
