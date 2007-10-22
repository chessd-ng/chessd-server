#include "Core.hh"
#include "CoreInterface.hh"
#include "MatchManager.hh"

#include <iostream>

Core* Core::_singleton = 0;

Core::Core(const XML::Tag& config_xml) :
	match_manager(config_xml.getChild("match-manager"), boost::bind(&Core::handleError, this, _1)),
	game_manager(config_xml.getChild("game-manager"), boost::bind(&Core::handleError, this, _1)) { }

void Core::connect() {
	this->match_manager.connect();
	this->game_manager.connect();
}

Core::~Core() { }

void Core::init(const XML::Tag& config_xml) {
	Core::_singleton = new Core(config_xml);
}

void Core::destroy() {
	delete Core::_singleton;
}

void Core::handleError(const std::string& error) {
	std::cerr << "Core ERROR " << error << std::endl;
}
