/*
 *   Copyright (c) 2007-2008 Raphael H. Ribas,  Eduardo A. Ribas.
 *
 *   This file is part of Chessd.
 *
 *   Chessd is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Chessd is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 */


#include "XML/iksutil.hh"

#include "Stream.hh"

#include <iostream>

#include <iksemel.h>

#include "boost/date_time/posix_time/posix_time.hpp"

using namespace std;
using namespace XML;

static void log_message(const std::string& msg) {
    cerr << boost::posix_time::second_clock::local_time() << ": " << msg << endl;
}

namespace XMPP {

	struct Stream::HookInfo {
		iksparser* parser;

		/*! \brief A queue of the incoming Tags */
		std::queue<Tag*> incoming;
	};

	static int hook(void* user_data, int type, iks* node) {
		if(node == 0)
			return IKS_OK;
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

	void Stream::connect(const string& host, int port) {
		int ret;
		ret = iks_connect_tcp(this->hinfo->parser, host.c_str(), port);
		this->active = (ret == IKS_OK);
		if(not this->active) {
			throw "Connection failed";
		}
	}

	void Stream::close() {
		iks_disconnect(this->hinfo->parser);
		this->active = false;
	}

	Tag* Stream::recvTag(int timeout) {
		int ret = IKS_OK;
		if(this->hinfo->incoming.empty()) {
			ret = iks_recv(this->hinfo->parser, timeout);
		}
		if(ret != IKS_OK) {
			this->active = false;
			iks_disconnect(this->hinfo->parser);
            throw "Connection lost";
		} else if(this->hinfo->incoming.empty()) {
			return 0;
		} else {
			Tag* tag = this->hinfo->incoming.front();
			this->hinfo->incoming.pop();
            log_message(" <<<<<<<<<<<< Chegando <<<<<<<<<<<<<");
			log_message(tag->xml());
			return tag;
		}
	}

	void Stream::sendTag(Tag* tag) {
        log_message(" >>>>>>>>>>>> Mandando >>>>>>>>>>>>>> ");
		log_message(tag->xml());
		iks* tree = tag2iks(*tag);
		delete tag;
		int ret = iks_send(this->hinfo->parser, tree);
		iks_delete(tree);
		if(ret != IKS_OK) {
			this->active = false;
			iks_disconnect(this->hinfo->parser);
			throw "Connection lost";
		}
	}

}
