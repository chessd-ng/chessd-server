#ifndef STREAM_LISTENER_HH
#define STREAM_LISTENER_HH

#include "XMPP/handlers.hh"
#include "XMPP/Component.hh"
#include "Threads/Task.hh"
#include "Threads/Queue.hh"


struct StreamListenerHandlers {
	XMPP::TagHandler handleTag;
	XMPP::ErrorHandler handleError;
	StreamListenerHandlers(
			const XMPP::TagHandler& handleTag,
			const XMPP::ErrorHandler& handleError) :
		handleTag(handleTag),
		handleError(handleError) { }
};

class StreamListener {
	public:
		StreamListener(const StreamListenerHandlers& handlers, XMPP::Stream& stream);

		~StreamListener();

		void start();

		void stop();

		void sendTag(XML::Tag* tag);

	private:
		StreamListenerHandlers handlers;
		XMPP::Stream& stream;
		bool running;

		Threads::Task task_recv;
		Threads::Task task_send;

		void run_recv();
		void run_send();

		Threads::Queue<XML::Tag*> tag_queue;
};

#endif
