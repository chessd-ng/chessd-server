#ifndef RATINGMANAGER_HH
#define RATINGMANAGER_HH

#include <map>
#include <set>
#include <memory>
#include "ComponentBase.hh"
#include "Query.hh"

class RatingManager : public ComponentBase {
	public:
		/*! \brief Constructor
		 *
		 * \param core_interface is the interface to the core.
		 * \param config is the configuration for this component.
		 */
		RatingManager(const XML::Tag& config, const XMPP::ErrorHandler& handleError);

		/*! \brief Destructor
		 *
		 * Closes server connection if available
		 */
		virtual ~RatingManager();

		/*! \brief Closes the conenction to the server */
		//void close();


	private:

		/* several handlers for the incoming events */

		/*! \brief handle an incoming match offer */
		void handleFetchRating(Query query);

        /*! \brief handle an incomint query  */
        void handleRatingQuery(Query Query);

		/*! \brief handle an incoming match iq */
		void handleRating(XMPP::Stanza* stanza);

		void notifyRating(Query* query, const XMPP::Jid& user, UserRatings* ratings);
};

#endif
