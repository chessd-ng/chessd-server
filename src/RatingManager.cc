#include <memory>
#include "RatingManager.hh"
#include "Util/utils.hh"

using namespace std;
using namespace XML;
using namespace XMPP;

RatingManager::RatingManager(const XML::Tag& config, const XMPP::ErrorHandler& handleError) :
    ComponentBase(config, handleError, "Rating Manager") {

        /* Set features */
        this->root_node.disco().features().insert("http://c3sl.ufpr.br/chessd#rating");

        /* Set rating iqs */
        this->root_node.setIqHandler(boost::bind(&RatingManager::handleRating, this, _1),
                "http://c3sl.ufpr.br/chessd#rating");

    }

RatingManager::~RatingManager() {
    //TODO
}

void RatingManager::handleRating(Stanza* _stanza) {
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

void RatingManager::handleRatingQuery(Query query) {
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

static XMPP::Jid parseFetchRating(const Query& query) {
    if(query.children().tags().begin() == query.children().tags().end())
        throw "Invalid format";
    const XML::Tag& tag = *query.children().tags().begin();
    if(not tag.hasAttribute("jid") or tag.name()!="rating")
        throw "Invalid format";
    return tag.getAttribute("jid");
}

void RatingManager::handleFetchRating(Query query) {
    try {
        XMPP::Jid user = parseFetchRating(query);
        this->core_interface.fetchUserRatings(
            user,
            boost::bind(&RatingManager::notifyRating, this, new Query(move(query)), user, _1));
    } catch (const char* msg) {
        this->component.sendStanza(
            Stanza::createErrorStanza(
                Query::createStanza(move(query)),
                    "cancel",
                    "bad-request"));
    }
}

void RatingManager::notifyRating(Query* _query, const XMPP::Jid& user, UserRatings* ratings) {
    auto_ptr<Query> query(_query);
    query->children().clear();
    foreach(rating, *ratings) {
        XML::Tag* tag = new Tag("rating");
        tag->setAttribute("jid", user.full());
        tag->setAttribute("style", rating->first);
        tag->setAttribute("rating", Util::int2str(rating->second.rating()));
        query->children().push_back(tag);
    }
    swap(query->from(), query->to());
    query->subtype() = "result";
    this->component.sendStanza(Query::createStanza(move(*query)));
}
