#include "ComponentBase.hh"
#include "Util/utils.hh"

using namespace std;
using namespace XML;
using namespace XMPP;

ComponentBaseParams::ComponentBaseParams(
    const std::string& component_name,
    const std::string& server_address,
    int server_port,
    const std::string& server_password) : 
        component_name(component_name),
        server_address(server_address),
        server_port(server_port),
        server_password(server_password) { }


ComponentBase::ComponentBase(const ComponentBaseParams& params,
		const std::string& component_name) :
	running(false),
	component(params.component_name),
	root_node(
            boost::bind(&ComponentBase::sendStanza, this, _1),
			XMPP::Jid(params.component_name),
			component_name, "service", "game"),
    params(params),
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
            this->params.server_address,
            this->params.server_port,
            this->params.server_password);
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
	this->dispatcher.queue(boost::bind(this->root_node.stanzaHandler(), stanza));
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
