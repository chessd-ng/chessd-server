#ifndef COMPONENTBASE_HH
#define COMPONENTBASE_HH

#include "XML/Xml.hh"

#include "CoreInterface.hh"
#include "XMPP/RootNode.hh"
#include "XMPP/Component.hh"
#include "Threads/Dispatcher.hh"
#include "Threads/Task.hh"
#include "Threads/Queue.hh"


/*! \brief A base implementation for components */
class ComponentBase {
	public:
		/*! \brief Constructor
		 *
		 * \param core_interface is the interface to the core.
		 * \param config is the configuration for this component.
		 */
		ComponentBase(const XML::Tag& config,
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
		void close();

        /*! \brief send s stanza to the server */
        void sendStanza(XMPP::Stanza* stanza);

    private:

		XMPP::Component component;

	protected:

		void _close();

        virtual void onClose() = 0;

        virtual void onError(const std::string& msg) = 0;

		void handleError(const std::string& error);

		void _handleError(const std::string& error);

		void handleStanza(XMPP::Stanza* stanza);

		/*! \brief We run in a separated thread as a dispatcher */
		Threads::Dispatcher dispatcher;

		/*! \brief Is it running? */
		bool running;

		/*! \brief Interface to the core */
		CoreInterface core_interface;

		/*! \brief A XMPP node*/
		XMPP::RootNode root_node;

	private:

		void run_recv();

		void run_send();

        std::string server_address;
        int server_port;
        std::string server_password;

		Threads::Task task_recv;

		Threads::Task task_send;

		Threads::Queue<XMPP::Stanza*> stanza_queue;


};

#endif
