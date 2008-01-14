#include <memory>
#include "RatingComponent.hh"
#include "Util/utils.hh"

using namespace std;
using namespace XML;
using namespace XMPP;

RatingComponentParams::RatingComponentParams(const XML::Tag& config_xml) :
    ComponentBaseParams(
        config_xml.getAttribute("node_name"),
        config_xml.getAttribute("server_address"),
        Util::str2int(config_xml.getAttribute("server_port")),
        config_xml.getAttribute("server_password")) { }

RatingComponent::RatingComponent(
        const RatingComponentParams& params,
        const XMPP::ErrorHandler& error_handler,
        RatingDatabase& rating_database) :
    ComponentBase(params, "Rating Component"),
    error_handler(error_handler),
    rating_database(rating_database)
{

    /* Set features */
    this->root_node.disco().features().insert("http://c3sl.ufpr.br/chessd#rating");

    /* Set rating iqs */
    this->root_node.setIqHandler(boost::bind(&RatingComponent::handleRating, this, _1),
            "http://c3sl.ufpr.br/chessd#rating");

}

RatingComponent::~RatingComponent() {
    //TODO
}

void RatingComponent::handleRating(Stanza* _stanza) {
    auto_ptr<Stanza> stanza(_stanza);
    try {
        Query query(move(*stanza));
        this->handleRatingQuery(move(query));
    } catch (const char* msg) {
        this->component.sendStanza(
            Stanza::createErrorStanza(
                stanza.release(),
                "cancel",
                "bad-request", msg));
    }
}

void RatingComponent::handleRatingQuery(Query query) {
    if(query.action() == "fetch") {
        this->handleFetchRating(move(query));
    } else {
        this->component.sendStanza(
            Stanza::createErrorStanza(
                Query::createStanza(move(query)),
                "cancel",
                "bad-request"));
    }
}

void checkFetchRating(const Query& query) {
    foreach(tag, query.children().tags()) {
        if(tag->name() != "rating" or not tag->hasAttribute("jid") or not tag->hasAttribute("category"))
            throw "Invalid format";
    }
}

void RatingComponent::handleFetchRating(Query query) {
    try {
        checkFetchRating(query);
        this->rating_database.perform_const(
            boost::bind(
                &RatingComponent::fetchRating,
                this,
                query,
                _1));
    } catch (const char* msg) {
        this->component.sendStanza(
            Stanza::createErrorStanza(
                Query::createStanza(move(query)),
                    "cancel",
                    "bad-request"));
    }
}

void RatingComponent::fetchRating(const Query& query, const RatingDBInterface& interface) {
    XML::TagGenerator generator;
    generator.openTag("iq");
    generator.addAttribute("to", query.from().full());
    generator.addAttribute("from", query.to().full());
    generator.addAttribute("id", query.id());
    generator.addAttribute("type", "result");
    generator.openTag("query");
    generator.addAttribute("xmlns", "http://c3sl.ufpr.br/chessd#rating");
    foreach(tag, query.children().tags()) {
        Rating rating = interface.getRating(tag->getAttribute("jid"), tag->getAttribute("category"));
        generator.openTag("rating");
        generator.addAttribute("jid", tag->getAttribute("jid"));
        generator.addAttribute("category", tag->getAttribute("category"));
        generator.addAttribute("rating", Util::int2str(rating.rating()));
        generator.addAttribute("wins", Util::int2str(rating.wins()));
        generator.addAttribute("draws", Util::int2str(rating.draws()));
        generator.addAttribute("losses", Util::int2str(rating.losses()));
        generator.closeTag();
    }
    generator.closeTag();
    this->component.sendStanza(new XMPP::Stanza(generator.closeTag()));
}

void RatingComponent::onError(const std::string& error) {
    this->error_handler(error);
}

void RatingComponent::onClose() {
}
