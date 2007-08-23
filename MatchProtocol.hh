#ifndef MATCHPROTOCOL_HH
#define MATCHPROTOCOL_HH

#include "XML/Xml.hh"

#include "Match.hh"

#include <memory>

/*! \brief The matchManagers's XMPP protocol. */
class MatchProtocol {
	public:
		/* \brief Constructor
		 *
		 * Loads all XML description of the messages.
		 * \param path is the path to the xml description files
		 */
		MatchProtocol(const std::string& path);

		/*! \brief Destructir */
		~MatchProtocol();

		/*! \brief Parse the incoming stanza.
		 *
		 * Throw an exception on error.
		 * \return A MatchOffer instance.
		 */
		MatchOffer* parseMatchOffer(XML::Tag& query);
		/*! Generate the answer query */
		XML::Tag* notifyMatchOffer(const Match& match, int id);

		/*! \brief Parse a match iq
		 *
		 * \return Returns the requested action
		 * */
		std::string parseMatch(XML::Tag& query);

		int parseMatchAccept(XML::Tag& query);

		int parseMatchDecline(XML::Tag& query);

		XML::Tag* notifyMatchResult(const Match& match, int id, bool accepted);

	private:
		XML::Description match_offer_desc;
		XML::Description match_desc;
		XML::Description match_accept_desc;
		XML::Description match_decline_desc;
};

#endif
