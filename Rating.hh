#ifndef RAINTG_HH
#define RATING_HH

class Rating {
	public:
		Rating() { }
		Rating(int rating, double sem_nome) : _rating(rating), _sem_nome(sem_nome) { }
		int rating() const { return this->_rating; }
		int& rating() { return this->_rating; }
		double volatility() const { return this->_volatility; }
		double& volatility() { return this->_volatility; }
	private:
		int _rating;
		double _volatility;
};

#endif
