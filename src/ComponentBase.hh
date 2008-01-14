#ifndef COMPONENTBASE_HH
#define COMPONENTBASE_HH

#include "CoreInterface.hh"
#include "XMPP/RootNode.hh"
#include "XMPP/Component.hh"
#include "Threads/Dispatcher.hh"
#include "Threads/Task.hh"
#include "Threads/Queue.hh"


struct ComponentBaseParams {
    std::string component_name;
    std::string server_address;
    int server_port;
    std::string server_password;

    ComponentBaseParams(
        const std::string& component_name,
        const std::string& server_address,
        int server_port,
        const std::string& server_password);
};

/*! \brief A base implementation for components */
class ComponentBase {
	public:
		/*! \brief Constructor
		 *
		 * \param core_interface is the interface to the core.
		 * \param config is the configuration for this component.
		 */
		ComponentBase(const ComponentBaseParams& params,
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

		/*! \brief A component wrapper */
		XMPP::Component component;

		/*! \brief A XMPP node*/
		XMPP::RootNode root_node;

	private:

		void run_recv();

		void run_send();

        ComponentBaseParams params;

		Threads::Task task_recv;

		Threads::Task task_send;

		Threads::Queue<XMPP::Stanza*> stanza_queue;

};

#endif
