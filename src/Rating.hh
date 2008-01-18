#ifndef RATING_HH
#define RATING_HH

class Rating {
	public:
		//FIXME check volatility
		Rating() : _rating(0), _volatility(0.0), _wins(0), _losses(0), _draws(0) { }
		Rating(int rating, double volatility) : _rating(rating), _volatility(volatility), _wins(0), _losses(0), _draws(0) { }

		int rating() const { return this->_rating; }
		int& rating() { return this->_rating; }

		double volatility() const { return this->_volatility; }
		double& volatility() { return this->_volatility; }

		int wins() const { return this->_wins; }
		int& wins() { return this->_wins; }

		int losses() const { return this->_losses; }
		int& losses() { return this->_losses; }

		int draws() const { return this->_draws; }
		int& draws() { return this->_draws; }

		int countGames() const { return this->_wins + this->_losses + this->_draws; }

	private:
		int _rating;
		double _volatility;
		int _wins, _losses, _draws;
};

#endif
