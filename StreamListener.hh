#ifndef STREAM_LISTENER_HH
#define STREAM_LISTENER_HH

#include "XMPP/handlers.hh"
#include "XMPP/Component.hh"
#include "Threads/Task.hh"

class StreamListener : public Threads::Task {
	public:
		StreamListener(const XMPP::TagHandler& handler, XMPP::Stream& stream);

		virtual ~StreamListener();

		virtual void run();

		void stop();

		void handleTag(XML::Tag* tag);

	private:
		TagHandler handler;
		Stream& stream;
		bool running;
};

#endif
