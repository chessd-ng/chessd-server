/*
 *   Copyright (c) 2007-2008 Raphael H. Ribas,  Eduardo A. Ribas.
 *
 *   This file is part of Chessd.
 *
 *   Chessd is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Chessd is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 */

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
