#ifndef MATCHADJOURNFACTORY_HH
#define MATCHADJOURNFACTORY_HH

#include "MatchStandard.hh"
#include "MatchBlitz.hh"
#include "MatchLightning.hh"

class MatchAdjournFactory {
	public:
		static Match* create(XML::Tag* history) {
			if(history->getAttribute("category")=="standard")
				return new MatchChessStandardAdjourn(history);
			else if(history->getAttribute("category")=="blitz")
				return new MatchChessBlitzAdjourn(history);
			else if(history->getAttribute("category")=="lightning")
				return new MatchChessLightningAdjourn(history);
			return 0;
		}
	private:
};

#endif
