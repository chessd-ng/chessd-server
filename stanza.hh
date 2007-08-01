#ifndef STANZA_HH
#define STANZA_HH

#include <string>
#include "jid.hh"
#include "xml.hh"

typedef std::vector<XML::Tag*> TagList;

enum ErrorType {
	ErrorCancel,
	ErrorContinue,
	ErrorModify,
	ErrorAuth,
	ErrorWait
};

enum DefinedCondition {
	ConditionBadRequest,
	ConditionConfilct,
	ConditionNotImplemented,
	ConditionForbidden,
	ConditionGone,
	ConditionInternal,
	ConditionNotFound,
	ConditionJID,
	ConditionNotAcceptable,
	ConditionNowAllowed,
	ConditionNotAuthorized,
	ConditionPayment,
	ConditionRecipientUnavailable,
	ConditionRedirect,
	ConditionRegister,
	ConditionServerNotFound,
	ConditionServerTimeout,
	ConditionResourceConstraint,
	ConditionServiceUnavailable,
	ConditionSubscribe,
	ConditionUndefined,
	ConditionUnexpected
};

class Stanza {
	public:
		Stanza(const std::string& type);

		/* WARNING this will destroy the original tag */
		Stanza(XML::Tag* tag);
		~Stanza();

		std::string& type() { return this->_type; }
		const std::string& type() const { return this->_type; }

		std::string& subtype() { return this->_subtype; }
		const std::string& subtype() const { return this->_subtype; }

		std::string& id() { return this->_id; }
		const std::string& id() const { return this->_id; }

		JID& from() { return this->_from; }
		const JID& from() const { return this->_from; }

		JID& to() { return this->_to; }
		const JID& to() const { return this->_to; }

		std::string& lang() { return this->_lang; }
		const std::string& lang() const { return this->_lang; }

		TagList& children() { return this->_children; }
		const TagList& children() const { return this->_children; }

		XML::Tag* findChild(const std::string& name);
		const XML::Tag* findChild(const std::string& name) const;

		std::string xml() const;

		Stanza* clone() const;

		/* this functions e helpers to read the tag */

		/* WARNING this will destroy the stanza */
		XML::Tag* tag();

		/* this functions are helpers to create stanzas */

		/* WARNING this will destroy the original stanza */
		static Stanza* createErrorStanza(Stanza* original,
				const std::string& error,
				const std::string& condition,
				const std::string& text = "");

	private:
		JID _from, _to;
		std::string _type, _subtype, _lang, _id;
		TagList _children;

};

#endif
