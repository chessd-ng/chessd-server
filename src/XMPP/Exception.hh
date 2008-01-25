#ifndef XMPPEXCEPTION_HH
#define XMPPEXCEPTION_HH

#include <stdexcept>

#include "Stanza.hh"

namespace XMPP {

    class xmpp_exception : public std::runtime_error {
        public:
            xmpp_exception(const std::string& msg) : runtime_error(msg) { }

            virtual Stanza* getErrorStanza(Stanza* stanza) const {
                return Stanza::createErrorStanza
                    (stanza, "cancel", "bad-request", this->what());
            }
    };

    class feature_not_implemented : public xmpp_exception {
        public:
            feature_not_implemented(const std::string& msg) : xmpp_exception(msg) { }
            
            virtual Stanza* getErrorStanza(Stanza* stanza) const {
                return Stanza::createErrorStanza
                    (stanza, "cancel", "feature-not-implemented", this->what());
            }
    };

    class not_acceptable : public xmpp_exception {
        public:
            not_acceptable(const std::string& msg) : xmpp_exception(msg) { }
            
            virtual Stanza* getErrorStanza(Stanza* stanza) const {
                return Stanza::createErrorStanza
                    (stanza, "cancel", "not-acceptable", this->what());
            }
    };

    class bad_request : public xmpp_exception {
        public:
            bad_request(const std::string& msg) : xmpp_exception(msg) { }
            
            virtual Stanza* getErrorStanza(Stanza* stanza) const {
                return Stanza::createErrorStanza
                    (stanza, "modify", "bad-request", this->what());
            }
    };

    class item_not_found : public xmpp_exception {
        public:
            item_not_found(const std::string& msg) : xmpp_exception(msg) { }
            
            virtual Stanza* getErrorStanza(Stanza* stanza) const {
                return Stanza::createErrorStanza
                    (stanza, "modify", "item-not-found", this->what());
            }
    };

}

#endif
