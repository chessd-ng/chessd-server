#include "jid.hh"

using namespace std;

namespace XMPP {

	Jid::Jid(const string& jid) {
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

	Jid::Jid(const Jid& jid) : _node(jid.node()), _domain(jid.domain()), _resource(jid.resource()) { }

	Jid::Jid() { }

	Jid::~Jid() { }

	string Jid::full() const {
		string resp;
		if(not this->node().empty())
			resp = this->node() + "@";
		resp += this->domain();
		if(not this->resource().empty())
			resp += "/" + this->resource();
		return resp;
	}

}
