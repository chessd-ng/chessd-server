#include "Core.hh"
#include "CoreInterface.hh"
#include "MatchManager.hh"

Core::Core(const XML::Tag* config_xml) :
	match_manager(new CoreInterface(*this),
			config_xml->getChild("match-manager"))
{
}

bool Core::connect() {
	return this->match_manager.connect();
}

Core::~Core() { }
