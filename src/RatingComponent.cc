#include <memory>
#include "RatingComponent.hh"
#include "Util/utils.hh"

#include "Exception.hh"

using namespace std;
using namespace XML;
using namespace XMPP;

RatingComponent::RatingComponent(
        const XML::Tag& config,
        const XMPP::ErrorHandler& error_handler,
        DatabaseManager& database) :
    ComponentBase(config, "Rating Component"),
    error_handler(error_handler),
    database(database)
{

    /* Set features */
    this->root_node.disco().features().insert("http://c3sl.ufpr.br/chessd#rating");

    /* Set rating iqs */
    this->root_node.setIqHandler(boost::bind(&RatingComponent::handleRating, this, _1),
            "http://c3sl.ufpr.br/chessd#rating");

}

RatingComponent::~RatingComponent() {
}

void RatingComponent::handleRating(Stanza* _stanza) {
    auto_ptr<Stanza> stanza(_stanza);
    try {
        try {
            Tag& query = stanza->findChild("query");
            const std::string& action = query.getAttribute("action");
            if(action == "fetch") {
                this->handleFetchRating(*stanza);
            } else {
                throw user_error("Invalid action - action is wrong");
            }
        } catch (const XML::child_not_found&) {
            throw user_error("Invalid message format - child not found");
        }

    } catch (const user_error& error) {
        this->sendStanza(
            Stanza::createErrorStanza(
                stanza.release(),
                "cancel",
                "bad-request",
                error.what()));
    }
}

void RatingComponent::handleFetchRating(const Stanza& stanza) {
    const Tag& query = stanza.findChild("query");

    /* check if the format is correct */
    foreach(tag, query.tags()) {
        if(tag->name() != "rating" or not tag->hasAttribute("category") or not tag->hasAttribute("jid"))
            throw user_error("Invalid message format - tag rating is wrong");
    }

    /* execute the transaction */
    this->database.queueTransaction(boost::bind(&RatingComponent::fetchRating, this, stanza, _1));
}

void RatingComponent::fetchRating(const Stanza& stanza, DatabaseInterface& database) {
    RatingDatabase& rating_database = database.rating_database;
    XML::TagGenerator generator;
    const Tag& query = stanza.findChild("query");
    generator.openTag("iq");
    generator.addAttribute("to", stanza.from().full());
    generator.addAttribute("from", stanza.to().full());
    generator.addAttribute("id", stanza.id());
    generator.addAttribute("type", "result");
    generator.openTag("query");
    generator.addAttribute("xmlns", "http://c3sl.ufpr.br/chessd#rating");
    foreach(tag, query.tags()) {
        Rating rating = rating_database.getRating
                (tag->getAttribute("jid"), tag->getAttribute("category"));
        generator.openTag("rating");
        generator.addAttribute("jid", tag->getAttribute("jid"));
        generator.addAttribute("category", tag->getAttribute("category"));
        generator.addAttribute("rating", Util::to_string(rating.rating()));
        generator.addAttribute("wins", Util::to_string(rating.wins()));
        generator.addAttribute("draws", Util::to_string(rating.draws()));
        generator.addAttribute("losses", Util::to_string(rating.losses()));
        generator.closeTag();
    }
    generator.closeTag();
    generator.closeTag();
    this->sendStanza(new XMPP::Stanza(generator.getTag()));
}

void RatingComponent::onError(const std::string& error) {
    this->error_handler(error);
}

void RatingComponent::onClose() {
}