#include "CoreInterface.hh"
#include "Core.hh"

CoreInterface::CoreInterface() : core(Core::singleton()) { }

CoreInterface::~CoreInterface() { }
