#ifndef COMPONENTWRAPPER_HH
#define COMPONENTWRAPPER_HH

#include "XMPP/Component.hh"
#include "XMPP/handlers.hh"
#include "StreamListener.hh"
#include "Threads/Dispatcher.hh"
#include "Threads/Semaphore.hh"

class ComponentWrapper {
	public:
		ComponentWrapper(
				const std::string& component_name,
				const XMPP::ErrorHandler& error_handler,
				const XMPP::StanzaHandler& root_handler);
		
		~ComponentWrapper();

		void sendStanza(XMPP::Stanza* stanza);

		void connect(const std::string& host_name, int host_port, const std::string& password);

		void close();

	private:
		void _close();

		void handleError(const std::string& error);
		void _handleError(const std::string error);

		void run_recv();
		void run_send();

		void onConnect();

		void _connect(const std::string& host_name, int host_port, const std::string& password);

		XMPP::Component component;

        XMPP::StanzaHandler root_handler;

		XMPP::ErrorHandler error_handler;

		std::string connection_error;

		Threads::Task task_recv;

		Threads::Task task_send;

		Threads::Dispatcher dispatcher;

		Threads::Queue<XMPP::Stanza*> stanza_queue;

		bool running;
};


#endif
