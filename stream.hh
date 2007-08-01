#ifndef STREAM_HH
#define STREAM_HH

#include <string>
#include "xml.hh"
#include "stanza.hh"
#include "sigc++/signal.h"
#include "handlers.hh"

class Stream : public sigc::trackable {
	public:
		Stream(const std::string& ns);
		~Stream();

		int connect(const std::string& host, int port);

		void send(XML::Tag* tag);

		void setTagHandler(const TagHandler& handler);

		void setLogHandler(const LogHandler& handler);

		int recv(int timeout = -1);

		struct HookInfo;
	private:

		HookInfo* hinfo;

		char* ns;
};

#endif
