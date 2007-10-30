#ifndef USER_RATING_HH
#define USER_RATING_HH

class UserStatus {
	public:
		const int& rating() const { return this->_rating; }
		int& rating() { return this->_rating; }

		const int& victories() const { return this->_victories; }
		int& victories() { return this->_victories; }

		const int& losses() const { return this->_losses; }
		int& losses() { return this->_losses; }

		const int& draws() const { return this->_draws; }
		int& draws() { return this->_draws; }

	private:
		int _rating;
		int _victories;
		int _losses;
		int _draws;
};

#endif
