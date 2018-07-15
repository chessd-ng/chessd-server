/*
 *   Copyright (c) 2007-2008 C3SL.
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

#include <iksemel.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "Util/Log.hh"

#include <stdexcept>

using namespace std;
using namespace XML;

namespace XMPP {

  namespace {
    int connect_socket(const char* host, int port) {

      struct hostent *hp;
      struct sockaddr_in sa;
      int arg, ret;

      /* resolve host address */
      if((hp = gethostbyname(host)) == NULL) {
        throw runtime_error("Unable to resolve host");
      }

      /* create the socket */
      int fd = socket(hp->h_addrtype, SOCK_STREAM, 0);
      if(fd == -1) {
        throw runtime_error("Unable to create socket");
      }

      /* set up host address */
      memset(&sa, 0, sizeof(sa));
      memcpy((char *)&sa.sin_addr, (char *)hp->h_addr, hp->h_length);
      sa.sin_family = hp->h_addrtype;
      sa.sin_port = htons(port);

      /* connect to the host */
      ret = connect(fd, (struct sockaddr *) &sa, sizeof sa);
      if(ret < 0 && errno != EINPROGRESS) {
        throw runtime_error("Unable to connect");
      }

      return fd;
    }
  }

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

	static void log(void* user_data, const char* data, size_t size, int is_incoming) {
		Stream::HookInfo* hinfo = (Stream::HookInfo*) user_data;
    string tmp(data, size);
    Util::log.log(tmp);
	}

	Stream::Stream(const string& ns) :
			hinfo(new HookInfo),
			ns(strdup(ns.c_str())),
			active(false) {
		this->hinfo->parser = iks_stream_new(this->ns, this->hinfo, hook);
		// iks_set_log_hook(this->hinfo->parser, log);
	}

	Stream::~Stream() {
		if(this->active) {
            this->close();
        }
		iks_parser_delete(this->hinfo->parser);
		delete this->hinfo;
		free(this->ns);
	}

	void Stream::connect(const string& host, int port, const string& to) {
    int fd = connect_socket(host.c_str(), port);

		int ret;
		ret = iks_connect_fd(this->hinfo->parser, fd);
		this->active = (ret == IKS_OK);

		if(not this->active) {
			throw runtime_error("Connection failed");
		}

    iks_send_header(this->hinfo->parser, to.c_str());
	}

    int Stream::getFD() const {
        return iks_fd(this->hinfo->parser);
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
			//iks_disconnect(this->hinfo->parser);
            throw runtime_error("Connection lost");
		} else if(this->hinfo->incoming.empty()) {
			return 0;
		} else {
			Tag* tag = this->hinfo->incoming.front();
			this->hinfo->incoming.pop();
            Util::log.log(" <<<<<<<<<<<< Receiving <<<<<<<<<<<<<");
			Util::log.log(tag->xml());
			return tag;
		}
	}

	void Stream::sendTag(Tag* tag) {
        Util::log.log(" >>>>>>>>>>>> Sending >>>>>>>>>>>>>> ");
		Util::log.log(tag->xml());
		iks* tree = tag2iks(*tag);
		delete tag;
		int ret = iks_send(this->hinfo->parser, tree);
		iks_delete(tree);
		if(ret != IKS_OK) {
			this->active = false;
			iks_disconnect(this->hinfo->parser);
			throw runtime_error("Connection lost");
		}
	}

}
