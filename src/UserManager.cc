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

#include "UserManager.hh"

using namespace std;
using namespace XMPP;
using namespace Threads;

UserManager::UserManager(const std::string& data_path, CoreInterface* core_interface) :
	component(),
	listener(component),
	root_node(boost::bind(&Component::send, &this->component, _1)),
	disco(boost::bind(&Component::send, &this->component, _1)),
	core_interface(core_interface),
	data_path(data_path)
{
	/* Set root_node to receive stanzas from the component */
	this->component.setRootStanzaHandler(boost:bind(&Node::handleStanza, &this->node, _1));
	/* Set disco's iq handlers */
	this->root_node.setIqHandler(boost::bind(&Disco::handleIqInfo, &this->disco, _1),
			"http://jabber.org/protocol/disco#info");
	this->root_node.setIqHandler(boost::bind(&Disco::handleIqInfo, &this->disco, _1),
			"http://jabber.org/protocol/disco#info");

	/* Set the disco to report the root node */
	DiscoNode disco_node("User Manager", "chess_server", "user");
	disco_node.features().insert("presence");

}
