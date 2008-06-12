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

#include "Core.hh"


Core* Core::_singleton = 0;

Core::Core(const XML::Tag& config_xml) :
    database_manager(config_xml.findChild("database")),
	game_manager(config_xml.findChild("game-component"),
        this->database_manager,
        boost::bind(&Core::handleError, this, _1)),
	match_manager(config_xml.findChild("match-component"),
        this->game_manager,
        this->database_manager,
        boost::bind(&Core::handleError, this, _1)),
	rating_component(config_xml.findChild("rating-component"),
        boost::bind(&Core::handleError, this, _1),
            this->database_manager),
	admin_component(config_xml.findChild("admin-component"),
        boost::bind(&Core::handleError, this, _1),
            this->database_manager)
{
}

void Core::start() {
    /* start all compoenents */
	this->dispatcher.start();
	this->match_manager.connect();
	this->game_manager.connect();
	this->rating_component.connect();
	this->admin_component.connect();
}

void Core::join() {
    this->dispatcher.join();
}

Core::~Core() {
    /* close all components */
	this->admin_component.close();
	this->rating_component.close();
	this->game_manager.close();
	this->match_manager.close();
	this->dispatcher.stop();
}

void Core::init(const XML::Tag& config_xml) {
    /* instanciate the core */
	Core::_singleton = new Core(config_xml);
}

void Core::close() {
	delete Core::_singleton;
}

void Core::handleError(const std::string& error) {
	std::cerr << "Core ERROR " << error << std::endl;
}
