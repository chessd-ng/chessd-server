#ifndef STREAM_LISTENER_HH
#define STREAM_LISTENER_HH

#include "handlers.hh"
#include "component.hh"
#include "task.hh"

class StreamListener : public Task, public sigc::trackable {
	public:
		StreamListener(const TagHandler& handler, Stream* stream);

		virtual ~StreamListener();

		virtual void run();

		void shutdown();

		void handleTag(XML::Tag* tag);

	private:
		TagHandler handler;
		Stream* stream;
		bool running;
};

#endif
