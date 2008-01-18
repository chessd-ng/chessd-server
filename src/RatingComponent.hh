#ifndef RATINGCOMPONENT_HH
#define RATINGCOMPONENT_HH

#include <map>
#include <set>
#include <memory>

#include "ComponentBase.hh"
#include "Query.hh"
#include "DatabaseManager.hh"

class RatingComponent : public ComponentBase {
	public:
		/*! \brief Constructor
		 *
		 * \param core_interface is the interface to the core.
		 * \param config is the configuration for this component.
		 */
		RatingComponent(
            const XML::Tag& config,
            const XMPP::ErrorHandler& handleError,
            DatabaseManager& database);

		/*! \brief Destructor
		 *
		 * Closes server connection if available
		 */
		virtual ~RatingComponent();

		/*! \brief Closes the conenction to the server */
		//void close();


	private:

        void onClose();

        void onError(const std::string& error);

		/* several handlers for the incoming events */

		/*! \brief handle an incoming match offer */
		void handleFetchRating(const XMPP::Stanza& query);

		/*! \brief handle an incoming match iq */
		void handleRating(XMPP::Stanza* stanza);

		void fetchRating(const XMPP::Stanza& stanza, DatabaseInterface& database);

        XMPP::ErrorHandler error_handler;

        DatabaseManager& database;
};

#endif
