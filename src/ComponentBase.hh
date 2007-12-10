#ifndef COMPONENTBASE_HH
#define COMPONENTBASE_HH

#include "CoreInterface.hh"
#include "XMPP/RootNode.hh"
#include "Threads/Dispatcher.hh"
#include "ComponentWrapper.hh"



/*! \brief A base implementation for components */
class ComponentBase {
	public:
		/*! \brief Constructor
		 *
		 * \param core_interface is the interface to the core.
		 * \param config is the configuration for this component.
		 */
		ComponentBase(const XML::Tag& config,
				const XMPP::ErrorHandler& handle_error,
				const std::string& component_name);

		/*! \brief Destructor
		 *
		 * Closes server connection if available
		 */
		virtual ~ComponentBase();

		/*! \brief Connect to the server.
		 *
		 * \throw Throws an exception on error.
		 */
		void connect();

		/*! \brief Closes the conenction to the server */
		virtual void close();


	protected:


		void handleError(const std::string& error);

		void handleStanza(XMPP::Stanza* stanza);

		/*! \brief We run in a separated thread as a dispatcher */
		Threads::Dispatcher dispatcher;

		/*! \brief Is it running? */
		bool running;

		/*! \brief Interface to the core */
		CoreInterface core_interface;

		/*! \brief A component wrapper */
		ComponentWrapper component;

		/*! \brief A XMPP node*/
		XMPP::RootNode root_node;

	private:
		/*! \brief The XMPP server address */
		std::string server_address;

		/*! \brief The server port */
		int server_port;

		/*! \brief The server password */
		std::string server_password;

		XMPP::ErrorHandler handle_error;
};

#endif
