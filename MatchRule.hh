#ifndef MATCHRULE_HH
#define MATCHRULE_HH

#include <vector>
#include <set>
#include <memory>
#include "Match.hh"
#include "Team.hh"
#include "TeamDatabase.hh"
#include "XML/Xml.hh"

/* This match stuff is not flexible as it should be according
 * to the OO design. Each rule may require very different 
 * parameters, so it should not be fixed to time and color.
 * As it fit to all use cases, it will stay this way. But,
 * later it should be changed. A consequence is that the
 * communication should have something equally flexible,
 * such as the X forms in the xmpp. */


/*struct MatchOffer {
	std::string category;
	std::vector<MatchPlayer> entities;
};*/

/*! \brief A MatchRule's descriptor .
 *
 * An abstract MatchRule is the one who tells if a match
 * is valid and if so it returns a Match instance */
class MatchRule {
	public:
		/*! \brief Destructor */
		virtual ~MatchRule() { }

		/*! \brief Returns the category name */
		virtual std::string getCategory() const = 0;

		/*! \brief check a given Match request.
		 *
		 * \param match_request is the requested match's description
		 * \param teamdb is the collection of available team
		 * \return Returns a pointer to a match description on success, 0 otherwise.
		 */
		virtual Match* checkOffer(XML::Tag& match_offer,
				const TeamDatabase& teams) const = 0;
};

#endif
