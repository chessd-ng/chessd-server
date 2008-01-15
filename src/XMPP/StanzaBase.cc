#include "StanzaBase.hh"
#include <algorithm>

using namespace std;

using namespace XMPP;

StanzaBase::StanzaBase(const StanzaBase& stanza_base) :
	_from(stanza_base.from()),
	_to(stanza_base.to()),
	_type(stanza_base.type()),
	_subtype(stanza_base.subtype()),
	_lang(stanza_base.lang()),
	_id(stanza_base.id()) { }

StanzaBase::StanzaBase(Moved<StanzaBase> stanza_base) {
	swap(_from,stanza_base->from());
	swap(_to,stanza_base->to());
	swap(_type,stanza_base->type());
	swap(_subtype,stanza_base->subtype());
	swap(_lang,stanza_base->lang());
	swap(_id,stanza_base->id());
}
