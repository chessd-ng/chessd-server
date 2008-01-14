#ifndef XMPPSTANZA_HH
#define XMPPSTANZA_HH

#include <string>
#include "Jid.hh"
#include "XML/Xml.hh"
#include "StanzaBase.hh"
#include "ChildrenList.hh"

#include <boost/ptr_container/ptr_vector.hpp>

namespace XMPP {

	typedef boost::ptr_vector<XML::Tag> TagList;

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
		ConditionJid,
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

	class Stanza : public StanzaBase {
		public:
			explicit Stanza(const std::string& type);

			explicit Stanza(XML::Tag* tag);

			explicit Stanza(Moved<XML::Tag> tag);

			explicit Stanza(Moved<StanzaBase> stanza_base);

			explicit Stanza(Moved<Stanza> stanza);

			Stanza(const Stanza& stanza);

			~Stanza();

			ChildrenList& children() { return this->_children; }
			const ChildrenList& children() const { return this->_children; }

			XML::Tag& findChild(const std::string& name);
			const XML::Tag& findChild(const std::string& name) const;

			void clearChildren();

			std::string xml() const;

			/* this functions are helpers to create stanzas */
			static Stanza* createErrorStanza(Stanza* original,
					const std::string& error,
					const std::string& condition,
					const std::string& text = "");

			static XML::Tag* createTag(Stanza* stanza);


			static Stanza* createIQResult(Stanza* stanza);

		private:
		    ChildrenList _children;

	};

}
#endif
