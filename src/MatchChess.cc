#include "MatchChess.hh"
#include "Util/utils.hh"

/*
 * MatchRuleChess Things
*/

MatchRuleChess::MatchRuleChess() {
}

MatchRuleChess::~MatchRuleChess() {
}

bool MatchRuleChess::validateXML(const XML::Tag& _match_offer) const {
	if(_match_offer.name()=="match")
	if(_match_offer.hasAttribute("category"))
	if(_match_offer.getAttribute("category") == this->getCategory()) {
		int conta=0;
		foreach(c_it,_match_offer.tags()) {
			if(c_it->name()=="player") {
				conta++;
				if(!(c_it->hasAttribute("jid") and \
							c_it->hasAttribute("color") and \
							c_it->hasAttribute("time")))
					return false;
				if(isTimeValid(*c_it)==false)
					return false;
			}
		}
		if(conta==2)
			return true;
	}
	return false;
}
StandardPlayerList MatchRuleChess::getPlayersfromXML(const XML::Tag& _match_offer) const {
	if(this->validateXML(_match_offer)==false)
		throw "Bad XML for MatchRuleChess";
	StandardPlayerList players;
	foreach(c_it,_match_offer.children()) {
		if(typeid(*c_it)==typeid(XML::Tag)) {
			const XML::Tag& tag_c = static_cast<const XML::Tag&>(*c_it);
			XMPP::Jid aux(tag_c.getAttribute("jid"));
			Util::Time time(tag_c.getAttribute("time"),Util::Minutes),inc;
			if(tag_c.hasAttribute("inc"))
				inc=Util::Time(tag_c.getAttribute("inc"),Util::Seconds);
			StandardPlayerColor c(tag_c.getAttribute("color")=="w"?White:Black);
			players.push_back(StandardPlayer(aux,time,inc,c));
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
		{
			t.addAttribute("jid",it->jid.node());

			char tmp[8];
			int tempo=(int)(it->time.getSeconds()+0.5);
			sprintf(tmp,"%d",tempo);
			t.addAttribute("time",std::string(tmp));

			tempo=(int)(it->inc.getSeconds()+0.5);
			sprintf(tmp,"%d",tempo);
			t.addAttribute("inc",std::string(tmp));

			t.addAttribute("color",it->color==White?"w":"b");
		}
		t.closeTag();
	}
	t.closeTag();
	return t.getTag();
}
