
#include "../XML/iksutil.hh"

#include "Stream.hh"

#include <iostream>

#include <iksemel.h>

using namespace std;
using namespace XML;

namespace XMPP {

	struct Stream::HookInfo {
		iksparser* parser;
		TagHandler tag_handler;
		LogHandler log_handler;
	};

	static int hook(void* user_data, int type, iks* node) {
		Stream::HookInfo* hinfo = (Stream::HookInfo*) user_data;
		Tag* tag = iks2tag(node);
		if(not hinfo->tag_handler.empty())
			hinfo->tag_handler(tag);
		else
			delete tag;
		iks_delete(node);
		return IKS_OK;
	}

	static void log(void* user_data, const char* data, size_t size, int is_incoming) {
		Stream::HookInfo* hinfo = (Stream::HookInfo*) user_data;
		if(not hinfo->log_handler.empty()) {
			string tmp(data, size);
			hinfo->log_handler(tmp, is_incoming);
		}
	}

	Stream::Stream(const string& ns) {
		this->hinfo = new HookInfo;
		this->ns = strdup(ns.c_str());
		this->hinfo->parser = iks_stream_new(this->ns, this->hinfo, hook);
		iks_set_log_hook(this->hinfo->parser, log);
	}

	Stream::~Stream() {
		iks_disconnect(this->hinfo->parser);
		iks_parser_delete(this->hinfo->parser);
		delete this->hinfo;
		free(this->ns);
	}

	int Stream::connect(const string& host, int port) {
		int error;
		error = iks_connect_tcp(this->hinfo->parser, host.c_str(), port);
		return error == IKS_OK ? 0: 1;
	}

	int Stream::recv(int timeout) {
		int error;
		error = iks_recv(this->hinfo->parser, timeout);
		return error;
	}

	void Stream::send(Tag* tag) {
		iks* tree = tag2iks(tag);
		delete tag;
		iks_send(this->hinfo->parser, tree);
		iks_delete(tree);
	}

	void Stream::setLogHandler(const LogHandler& handler) {
		this->hinfo->log_handler = handler;
	}

	void Stream::setTagHandler(const TagHandler& handler) {
		this->hinfo->tag_handler = handler;
	}

}
