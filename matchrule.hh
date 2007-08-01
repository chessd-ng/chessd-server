#ifndef MATCHRULE_HH
#define MATCHRULE_HH

#include <vector>
#include <set>
#include "time.hh"
#include "match.hh"
#include "sdb.hh"

/* This match stuff is not flexible as it should be according
 * to the OO design. Each rule may require very different 
 * parameters, so it should not be fixed to time and color.
 * As it fit to all use cases, it will stay this way. But,
 * later it should be changed. A consequence is that the
 * communication should have something equally flexible,
 * such as the X forms in the xmpp. */

/*  */
typedef std::vector<MatchEntity> MatchRequest;

/* An abstract MatchRule is the one who tells if a match
 * is valid and if so it returns a Match instance */
class MatchRule {
	public:
		virtual ~MatchRule() { }
		virtual std::string name() = 0;
		virtual Match* checkMatchRequest(const MatchRequest& match_request,
				const SimpleDatabase<Team>&) = 0;
};

#endif
