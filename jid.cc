#include "jid.hh"

using namespace std;

JID::JID(const string& jid) {
	size_t arroba = jid.find('@');
	if(arroba != string::npos) {
		this->node() = jid.substr(0, arroba);
		arroba ++;
	} else {
		arroba = 0;
	}
	size_t barra = jid.find('/', arroba);
	if(barra != string::npos) {
		this->resource() = jid.substr(barra + 1);
	} else {
		barra = jid.size();
	}
	this->domain() = jid.substr(arroba, barra - arroba);
}

JID::JID(const JID& jid) : _node(jid.node()), _domain(jid.domain()), _resource(jid.resource()) { }

JID::JID() { }

JID::~JID() { }

string JID::full() const {
	string resp;
	if(not this->node().empty())
		resp = this->node() + "@";
	resp += this->domain();
	if(not this->resource().empty())
		resp += "/" + this->resource();
	return resp;
}
