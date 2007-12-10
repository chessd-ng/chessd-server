#ifndef XMPPROOTNODE_HH
#define XMPPROOTNODE_HH

#include <map>
#include "Node.hh"
#include "Jid.hh"


namespace XMPP {

	class RootNode : public Node {
		public:
			/*! \brief Constructor
			 *
			 * \param sender is a function to be used to send a stanza.
			 * \param category is the node category according to disco spec.
			 * \param type if the node type.
			 * \param name is the node name.
			 */
			RootNode(const StanzaHandler& send_stanza,
					const Jid& jid,
					const std::string& name,
					const std::string& category,
					const std::string& type);
			~RootNode();

			/*! \brief Send a stanza to this  node.
			 *
			 * \param stanza is the incoming stanza.
			 */
			void handleStanza(Stanza* stanza);

			/*! \brief set a handler for a node
			 *
			 */
			void setNodeHandler(const std::string& node_name, const StanzaHandler& handler);

			void removeNodeHandler(const std::string& node_name);

		private:
			typedef std::map<std::string, StanzaHandler> HandlerMap;
			std::map<std::string, StanzaHandler> node_handlers;

			struct NodeStanzaHandler : std::unary_function<Stanza*, void> {
				public:
					NodeStanzaHandler(RootNode& root_node) : root_node(root_node) { }
					void operator()(Stanza* stanza) { root_node.handleStanza(stanza); }
				private:
					RootNode& root_node;
			};
		public:
			NodeStanzaHandler stanzaHandler() {
				return NodeStanzaHandler(*this);
			}
	};

}

#endif
