#ifndef GAMESTANDARD_HH
#define GAMESTANDARD_HH

#include "GameChess.hh"

//TODO ver negocios do tempo!
class GameStandard : public GameChess {
	public:
		//FIXME ver se a passagem pode ser feita por referencia
		GameStandard(const StandardPlayerList& _players);

		virtual ~GameStandard() {};

		virtual const std::string& category() const;

	private:
		std::string _category;
};

#endif
