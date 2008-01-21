#ifndef MATCHPROTOCOL_HH
#define MATCHPROTOCOL_HH

#include "XML/Xml.hh"

#include "Match.hh"

#include <memory>

/*! \brief The matchManagers's XMPP protocol. */
class MatchProtocol {
	public:
		/*! \brief Parse the incoming stanza.
		 *
		 * Throw an exception on error.
		 * \return A MatchOffer instance.
		 */
		//static MatchOffer* parseMatchOffer(XML::Tag& query);

		/*! Generate the answer query */
		static XML::Tag* notifyMatchOffer(const Match& match, int id);

		/*! \brief Parse a match iq
		 *
		 * \return Returns the requested action
		 * */
		static std::string parseMatch(XML::Tag& query);

		static int parseMatchAccept(XML::Tag& query);

		static int parseMatchDecline(XML::Tag& query);

		static XML::Tag* notifyMatchResult(const Match& match, int id, bool accepted);

		/* \brief Initializer
		 *
		 * Loads all XML description of the messages.
		 * \param path is the path to the xml description files
		 */
		static void init(const std::string& path);

	private:
		/* \brief Constructor */
		MatchProtocol() { }

		/*! \brief Destructir */
		~MatchProtocol() { }

		static XML::Description match_offer_desc;
		static XML::Description match_desc;
		static XML::Description match_accept_desc;
		static XML::Description match_decline_desc;
};

#endif