#ifndef XMPPDISCO_HH
#define XMPPDISCO_HH

#include "Jid.hh"
#include "handlers.hh"

#include <set>
#include <map>

namespace XMPP {

	typedef std::set<std::string> StringSet;

	/*! \brief A XMPP disco item. */
	struct DiscoItem {
		const std::string name;
		const Jid jid;
		DiscoItem(const std::string& name,
				const Jid& jid);
		bool operator<(const DiscoItem& item) const;
	};

	typedef std::set<DiscoItem> DiscoItemSet;

	/*! \brief A implementation of the XMPP Disco extension. */
	class Disco {
		public:
			/*! \brief Costructor
			 *
			 * \param sender is a function used to send the stanzas
			 */
			Disco(const StanzaSender& sender,
					const std::string& name = "",
					const std::string& category = "",
					const std::string& type = "");
			~Disco();

			/*! \brief Handler for the info
			 *
			 * The XMPP especifies that this iqs has 
			 * "http://jabber.org/protocol/disco#info" as xmlns
			 * */
			void handleIqInfo(Stanza* stanza);
			/*! \brief Handler for the items
			 *
			 * The XMPP especifies that this iqs has 
			 * "http://jabber.org/protocol/disco#items" as xmlns
			 * */
			void handleIqItems(Stanza* stanza);


			/*! \brief The node's category */
			const std::string& category() const { return this->_category; }
			//std::string& category() { return this->_category; }

			/*! \brief The node's type */
			const std::string& type() const { return this->_type; }
			//std::string& type() { return this->_type; }

			/*! \brief The node's name */
			const std::string& name() const { return this->_name; }
			//std::string& name() { return this->_name; }

			/*! \brief The node's features */
			const StringSet& features() const { return this->_features; }
			/*! \brief The node's features */
			StringSet& features() { return this->_features; }

			/*! \brief The node's items */
			const DiscoItemSet& items() const { return this->_items; }
			/*! \brief The node's items */
			DiscoItemSet& items() { return this->_items; }

		private:

			StanzaSender stanza_sender;

			const std::string _name, _category, _type;

			StringSet _features;

			DiscoItemSet _items;

	};

}
#endif
