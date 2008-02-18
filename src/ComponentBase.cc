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

#include "ComponentBase.hh"
#include "Util/utils.hh"

using namespace std;
using namespace XML;
using namespace XMPP;
using namespace Util;

ComponentBase::ComponentBase(const XML::Tag& config,
		const std::string& component_name) :
	component(config.getAttribute("node_name")),
	running(false),
	root_node(
            boost::bind(&ComponentBase::sendStanza, this, _1),
			XMPP::Jid(config.getAttribute("node_name")),
			component_name, "service", "game"),
    server_address(config.getAttribute("server_address")),
    server_port(parse_string<int>(config.getAttribute("server_port"))),
    server_password(config.getAttribute("server_password")),
	task_recv(boost::bind(&ComponentBase::run_recv, this)),
	task_send(boost::bind(&ComponentBase::run_send, this))
{
	this->dispatcher.start();
}

ComponentBase::~ComponentBase() {
	this->close();
	this->dispatcher.stop();
}

void ComponentBase::connect() {
    this->component.connect(
            this->server_address,
            this->server_port,
            this->server_password);
    this->running = true;
    this->task_recv.start();
    this->task_send.start();
}

void ComponentBase::close() {
    this->dispatcher.queue(boost::bind(&ComponentBase::_close, this));
}

void ComponentBase::_close() {
	if(this->running) {
        this->onClose();
		this->running = false;
		this->stanza_queue.push(0);
		this->task_recv.join();
		this->task_send.join();
		this->component.close();
	}
}

void ComponentBase::handleError(const std::string& error) {
    this->dispatcher.queue(boost::bind(&ComponentBase::_handleError, this, error));
}

void ComponentBase::_handleError(const std::string& error) {
    this->onError(error);
    this->_close();
}

void ComponentBase::handleStanza(XMPP::Stanza* stanza) {
	this->dispatcher.queue(boost::bind(&XMPP::RootNode::handleStanza, &this->root_node, stanza));
}

void ComponentBase::run_recv() {
    XMPP::Stanza* stanza;
	try {
		while(this->running) {
			stanza = this->component.recvStanza(1);
            if(stanza != 0) {
                this->handleStanza(stanza);
            }
		}
	} catch (const char* error) {
		if(this->running)
			this->handleError(error);
	}
}

void ComponentBase::run_send() {
	XMPP::Stanza* stanza;
	while(this->running) {
		stanza = this->stanza_queue.pop();
		if(stanza==0)
			break;
        try {
            this->component.sendStanza(stanza);
        } catch (const char* error) {
            this->handleError(error);
        }
	}
}

void ComponentBase::sendStanza(XMPP::Stanza* stanza) {
    this->stanza_queue.push(stanza);
}
