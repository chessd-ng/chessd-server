#ifndef COMPONENTLISTENER_HH
#define COMPONENTLISTENER_HH

#include "XMPP/handlers.hh"
#include "XMPP/Component.hh"
#include "Threads/Task.hh"

class ComponentListener {
	public:
		ComponentListener(XMPP::Component& component);

		~ComponentListener();

		void start();


		void stop();

	private:
		XMPP::Component& component;

		bool running;

		Threads::Task task;

		void run();
};

#endif
