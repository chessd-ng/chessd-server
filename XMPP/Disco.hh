#ifndef XMPPDISCO_HH
#define XMPPDISCO_HH

#include "Jid.hh"
#include "handlers.hh"

#include <set>
#include <map>

namespace XMPP {

	typedef std::set<std::string> StringSet;

	struct DiscoItem {
		const std::string name;
		const Jid jid;
		const std::string node;
		DiscoItem(const std::string& name,
				const Jid& jid,
				const std::string& node = "");
		bool operator<(const DiscoItem& item) const;
	};

	typedef std::set<DiscoItem> DiscoItemSet;


	class DiscoNode {
		public:
			DiscoNode(const std::string& name,
					const std::string& category,
					const std::string& type);
			~DiscoNode();

			const std::string& category() const { return this->_category; }
			//std::string& category() { return this->_category; }

			const std::string& type() const { return this->_type; }
			//std::string& type() { return this->_type; }

			const std::string& name() const { return this->_name; }
			//std::string& name() { return this->_name; }

			const StringSet& features() const { return this->_features; }
			StringSet& features() { return this->_features; }

			const DiscoItemSet& items() const { return this->_items; }
			DiscoItemSet& items() { return this->_items; }

		private:

			const std::string _name, _category, _type;

			StringSet _features;

			DiscoItemSet _items;
	};

	typedef std::map<std::pair<Jid, std::string>, DiscoNode*> DiscoNodeMap;

	class Disco {
		public:
			Disco(const StanzaSender& sender);
			~Disco();

			void handleIqInfo(Stanza* stanza);
			void handleIqItems(Stanza* stanza);

			const DiscoNodeMap& nodes() const { return this->_nodes; }
			DiscoNodeMap& nodes() { return this->_nodes; }

		private:

			DiscoNodeMap _nodes;

			StanzaSender stanza_sender;
	};

}
#endif
