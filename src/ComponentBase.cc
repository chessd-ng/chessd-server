#include "ComponentBase.hh"
#include "Util/utils.hh"

using namespace std;
using namespace XML;
using namespace XMPP;

ComponentBase::ComponentBase(const XML::Tag& config,
		const XMPP::ErrorHandler& handleError,
		const std::string& component_name) :
	running(false),
	component(
			ComponentWrapperHandlers(boost::bind(&ComponentBase::handleError, this, _1)),
			config.getAttribute("node_name"), 
			boost::bind(&ComponentBase::handleStanza, this, _1)),
	root_node(boost::bind(&ComponentWrapper::sendStanza, &this->component, _1),
			XMPP::Jid(config.getAttribute("node_name")),
			component_name, "service", "game"),
	server_address(config.getAttribute("server_address")),
	server_port(Util::str2int(config.getAttribute("server_port"))),
	server_password(config.getAttribute("server_password")),
	handle_error(handle_error)
{
	this->dispatcher.start();
}

ComponentBase::~ComponentBase() {
	this->close();
	this->dispatcher.stop();
}

void ComponentBase::connect() {
	this->running = true;
	this->component.connect(this->server_address, this->server_port, this->server_password);
}

void ComponentBase::close() {
	if(this->running) {
		this->running = false;
		this->component.close();
	}
}

void ComponentBase::handleError(const std::string& error) {
	this->close();
	this->handle_error(error);
}

void ComponentBase::handleStanza(XMPP::Stanza* stanza) {
	this->dispatcher.queue(boost::bind(this->root_node.stanzaHandler(), stanza));
}
