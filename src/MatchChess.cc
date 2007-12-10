#include "MatchChess.hh"
#include "Util/utils.hh"

/*
 * MatchRuleChess Things
*/

MatchRuleChess::MatchRuleChess() {
}

MatchRuleChess::~MatchRuleChess() {
}

StandardPlayerList MatchRuleChess::getPlayersfromXML(const XML::Tag& _match_offer) const {
/*	Description d;
	d.loadFromFile("");
	if(!d.validateXML(_mattch_offer))
		return 0;
	if(_mattch_offer.attributes()["category"]!="standard")
		return 0;
*/
	//TODO verify rated game
	//TODO verify time
	StandardPlayerList players;
	foreach(c_it,_match_offer.children()) {
		if(typeid(*c_it)==typeid(XML::Tag)) {
			const XML::Tag& tag_c = static_cast<const XML::Tag&>(*c_it);
			XMPP::Jid aux(tag_c.getAttribute("jid"));
			Util::Time t;
			StandardPlayerColor c(tag_c.getAttribute("color")=="w"?White:Black);
			players.push_back(StandardPlayer(aux,t,t,c));
		}
	}
	return players;
}

/*
 * MatchChess things
*/

MatchChess::MatchChess(const StandardPlayerList &players) : 
	_match_players(players)
{
	foreach(it,players)
		this->_players.push_back(Player(it->jid));
}

MatchChess::~MatchChess(){
}

const PlayerList& MatchChess::players() const {
	return this->_players;
}


XML::Tag* MatchChess::notification() const {
	XML::TagGenerator t;
	t.openTag("match");
	t.addAttribute("category",this->category());
	foreach(it,_match_players) {
		t.openTag("player");
		t.addAttribute("jid",it->jid.node());
		t.addAttribute("time","11"); //FIXME How to transform Time to string?
		t.addAttribute("inc","1");
		t.addAttribute("color",it->color==White?"w":"b");
		t.closeTag();
	}
	return t.closeTag();
}
