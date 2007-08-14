#ifndef COMPONENTLISTENER_HH
#define COMPONENTLISTENER_HH

#include "XMPP/handlers.hh"
#include "XMPP/Component.hh"
#include "Threads/Task.hh"

class ComponentListener : public Threads::Task {
	public:
		ComponentListener(XMPP::Component& component);

		virtual ~ComponentListener();

		virtual void run();

		void stop();

	private:
		XMPP::Component& component;
		bool running;
};

#endif
