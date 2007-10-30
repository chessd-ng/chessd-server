#ifndef COMPONENTWRAPPER_HH
#define COMPONENTWRAPPER_HH

#include "XMPP/Stream.hh"
#include "XMPP/Component.hh"
#include "XMPP/handlers.hh"
#include "StreamListener.hh"
#include "Threads/Dispatcher.hh"
#include "Threads/Semaphore.hh"

struct ComponentWrapperHandlers {
	XMPP::ErrorHandler handleError;

	ComponentWrapperHandlers(
			const XMPP::ErrorHandler& handleError) :
		handleError(handleError) { }
};

class ComponentWrapper {
	public:
		ComponentWrapper(
				ComponentWrapperHandlers handlers,
				const std::string& component_name,
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

		XMPP::Stream stream;

		XMPP::Component component;

		ComponentWrapperHandlers handlers;

		std::string connection_error;

		Threads::Task task_recv;

		Threads::Task task_send;

		Threads::Dispatcher dispatcher;

		Threads::Queue<XMPP::Stanza*> stanza_queue;

		bool running;
};


#endif
