
#include "../XML/iksutil.hh"

#include "Stream.hh"

#include <iostream>

#include <iksemel.h>

using namespace std;
using namespace XML;

namespace XMPP {

	struct Stream::HookInfo {
		iksparser* parser;

		/*! \brief A queue of the incoming Tags */
		std::queue<Tag*> incoming;
	};

	static int hook(void* user_data, int type, iks* node) {
		Stream::HookInfo* hinfo = (Stream::HookInfo*) user_data;
		Tag* tag = iks2tag(node);
		iks_delete(node);
		hinfo->incoming.push(tag);
		return IKS_OK;
	}

	/*static void log(void* user_data, const char* data, size_t size, int is_incoming) {
		Stream::HookInfo* hinfo = (Stream::HookInfo*) user_data;
		if(not hinfo->log_handler.empty()) {
			string tmp(data, size);
			hinfo->log_handler(tmp, is_incoming);
		}
	}*/

	Stream::Stream(const string& ns) :
			hinfo(new HookInfo),
			ns(strdup(ns.c_str())),
			active(false)  {
		this->hinfo->parser = iks_stream_new(this->ns, this->hinfo, hook);
		//iks_set_log_hook(this->hinfo->parser, log);
	}

	Stream::~Stream() {
		if(this->active)
			iks_disconnect(this->hinfo->parser);
		iks_parser_delete(this->hinfo->parser);
		delete this->hinfo;
		free(this->ns);
	}

	bool Stream::connect(const string& host, int port) {
		int ret;
		ret = iks_connect_tcp(this->hinfo->parser, host.c_str(), port);
		this->active = (ret == IKS_OK);
		return this->active;
	}

	void Stream::close() {
		iks_disconnect(this->hinfo->parser);
		this->active = false;
	}

	Tag* Stream::recv(int timeout) {
		int ret = IKS_OK;
		if(this->hinfo->incoming.empty()) {
			ret = iks_recv(this->hinfo->parser, timeout);
		}
		if(ret != IKS_OK) {
			this->active = false;
			iks_disconnect(this->hinfo->parser);
			return 0;
		} else if(this->hinfo->incoming.empty()) {
			return 0;
		} else {
			Tag* tag = this->hinfo->incoming.front();
			this->hinfo->incoming.pop();
			cout << tag->xml() << endl;
			return tag;
		}
	}

	bool Stream::send(Tag* tag) {
		cout << tag->xml() << endl;
		iks* tree = tag2iks(tag);
		delete tag;
		int ret = iks_send(this->hinfo->parser, tree);
		iks_delete(tree);
		if(ret != IKS_OK) {
			this->active = false;
			iks_disconnect(this->hinfo->parser);
		}
		return this->active;
	}
}
