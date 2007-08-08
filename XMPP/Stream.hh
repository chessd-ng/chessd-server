#ifndef XMPPSTREAM_HH
#define XMPPSTREAM_HH


#include "../XML/Xml.hh"

#include "handlers.hh"

#include <boost/function.hpp>

#include <string>

namespace XMPP {

	class Stream {
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

}

#endif
