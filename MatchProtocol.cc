#include "MatchProtocol.hh"
#include "Util/utils.hh"
#include <memory>

using namespace std;

XML::Description MatchProtocol::match_offer_desc;
XML::Description MatchProtocol::match_desc;
XML::Description MatchProtocol::match_accept_desc;
XML::Description MatchProtocol::match_decline_desc;

void MatchProtocol::init(const std::string& path) {
	if(not MatchProtocol::match_offer_desc.loadFromFile(path + "/match_offer.desc"))
		throw "Could not load match_offer.desc";
	if(not MatchProtocol::match_desc.loadFromFile(path + "/match.desc"))
		throw "Could not load match.desc";
	if(not MatchProtocol::match_accept_desc.loadFromFile(path + "/match_accept.desc"))
		throw "Could not load match_accept.desc";
	if(not MatchProtocol::match_decline_desc.loadFromFile(path + "/match_decline.desc"))
		throw "Could not load match_decline.desc";
}

MatchOffer* MatchProtocol::parseMatchOffer(XML::Tag& query) {
	/* Validate format */
	if(not MatchProtocol::match_offer_desc.validateXML(query))
		throw "Invalid syntax";
	/* Get the query's children */
	XML::ChildrenList::iterator it = query.children().begin();
	/* Create de offer, make sure it will be deleted on exception */
	auto_ptr<MatchOffer> offer(new MatchOffer);
	/* Get the category */
	XML::Tag& tag = dynamic_cast<XML::Tag&> (*it);
	offer->category = tag.getAttribute("category");
	/* Iterate through players */
	for(++it; it != query.children().end(); ++it) {
		XML::Tag& tag=dynamic_cast<XML::Tag&> (*it);
		/* check if the color is valid */
		if(tag.getAttribute("color")!="w" and tag.getAttribute("color")!="b")
			throw "Invalid color";

		offer->entities.push_back(MatchPlayer(tag.getAttribute("jid"),
					Util::str2int(tag.getAttribute("time")) * 60 * Util::Seconds,
					Util::str2int(tag.getAttribute("inc")) * Util::Seconds,
					tag.getAttribute("color")=="w"?White:Black));
	}
	return offer.release();
}

std::string MatchProtocol::parseMatch(XML::Tag& query) {
	/* Validate format */
	if(not MatchProtocol::match_desc.validateXML(query))
		throw "Invalid syntax";
	return query.getAttribute("action");
}

int MatchProtocol::parseMatchAccept(XML::Tag& query) {
	if(not MatchProtocol::match_accept_desc.validateXML(query))
		throw "Invalid syntax";
	return Util::str2int(query.getChild("match").getAttribute("id"));
}

int MatchProtocol::parseMatchDecline(XML::Tag& query) {
	if(not MatchProtocol::match_decline_desc.validateXML(query))
		throw "Invalid syntax";
	return Util::str2int(query.getChild("match").getAttribute("id"));
}

XML::Tag* MatchProtocol::notifyMatchOffer(const Match& match, int id) {
	XML::TagGenerator generator;
	generator.openTag("query");
	generator.addAttribute("xmlns", "http://c3sl.ufpr.br/chessd#match");
	generator.addAttribute("action", "offer");
	generator.openTag("match");
	generator.addAttribute("id", Util::int2str(id));
	generator.addAttribute("category", match.getCategory());
	generator.closeTag();
	foreach(team, match.getTeams()) {
		generator.openTag("team");
		foreach(player, *team) {
			generator.openTag("player");
			generator.addAttribute("jid", player->jid.full());
			generator.addAttribute("time", Util::int2str(int(player->time.getSeconds())));
			generator.addAttribute("inc", Util::int2str(int(player->inc.getSeconds())));
			generator.addAttribute("color", player->color == White ? "w" : "b");
			generator.closeTag();
		}
		generator.closeTag();
	}
	return generator.closeTag();
}

XML::Tag* MatchProtocol::notifyMatchResult(const Match& match, int id, bool accepted) {
	XML::TagGenerator generator;
	generator.openTag("query");
	generator.addAttribute("xmlns", "http://c3sl.ufpr.br/chessd#match");
	generator.addAttribute("action", accepted?"accept":"decline");
	generator.openTag("match");
	generator.addAttribute("id", Util::int2str(id));
	generator.closeTag();
	return generator.closeTag();
}
