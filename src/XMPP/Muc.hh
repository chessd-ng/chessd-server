/*
 *   Copyright (c) 2007-2008 Raphael H. Ribas,  Eduardo A. Ribas.
 *
 *   This file is part of Chessd.
 *
 *   Chessd is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Chessd is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 */

#ifndef XMPPMUC_HH
#define XMPPMUC_HH

#include "Disco.hh"
#include "Node.hh"
#include "handlers.hh"
#include "Util/utils.hh"

#include <vector>
#include <string>


namespace XMPP {

    struct MucUser {
        public:
            MucUser(const std::string& nick, const std::string& affiliation,
                    const std::string& role, const Jid& jid);

            std::string& nick() { return this->_nick;}
            const std::string& nick() const { return this->_nick;}

            std::string& affiliation() { return this->_affiliation;}
            const std::string& affiliation() const { return this->_affiliation;}

            std::string& role() { return this->_role;}
            const std::string& role() const { return this->_role;}

            XMPP::Jid& jid() { return this->_jid;}
            const XMPP::Jid& jid() const { return this->_jid;}


        private:
            std::string _nick, _affiliation, _role;
            Jid _jid;


    };

	struct MucUserSet {
		private:
			typedef boost::ptr_map<Jid, MucUser> user_map;
			typedef boost::ptr_map<Jid, MucUser>::iterator iterator_map;
			typedef boost::ptr_map<Jid, MucUser>::const_iterator const_iterator_map;
			struct iterator_transformer :
				public std::unary_function<user_map::value_type, MucUser> {
				MucUser& operator()(const user_map::value_type& v) const {
					return *v.second;
				}
			};
			struct const_iterator_transformer :
				public std::unary_function<user_map::value_type, MucUser> {
				const MucUser& operator()(const user_map::value_type& v) {
					return *v.second;
				}
			};
		public:
			MucUserSet() { }
			~MucUserSet() { }

			typedef boost::transform_iterator<MucUserSet::iterator_transformer, iterator_map> iterator;
			typedef boost::transform_iterator<MucUserSet::const_iterator_transformer, const_iterator_map> const_iterator;

			iterator begin() {
				return iterator(this->users.begin());
			}
			iterator end() {
				return iterator(this->users.end());
			}
			const_iterator begin() const {
				return const_iterator(this->users.begin());
			}
			const_iterator end() const {
				return const_iterator(this->users.end());
			}
            int size() const {
                return this->users.size();
            }
			void insert(MucUser* user) {
				Jid jid = user->jid();
				this->users.insert(jid, user);
			}
			void erase(const Jid& user_jid) {
				this->users.erase(user_jid);
			}
			void erase(iterator user) {
				this->users.erase(user.base());
			}
			const_iterator find_jid(const Jid& user_jid) const {
				return const_iterator(this->users.find(user_jid));
			}
			iterator find_jid(const Jid& user_jid) {
				return iterator(this->users.find(user_jid));
			}
			// XXX
			const_iterator find_nick(const std::string& user_nick) const {
				foreach(it, this->users)
					if(it->second->nick() == user_nick)
						return const_iterator(it);
				return const_iterator(this->users.end());
			}
			iterator find_nick(const std::string& user_nick) {
				foreach(it, this->users)
					if(it->second->nick() == user_nick)
						return iterator(it);
				return iterator(this->users.end());
			}
		private:
			user_map users;
	};

    typedef boost::function<void (const Jid&, const std::string&, bool)> OccupantMonitor;

	class Muc : public Node {
		public:
			Muc(const Jid& jid,
                    const std::string& room_title,
                    StanzaHandler send_stanza);

			~Muc();

            void broadcast(Stanza* stanza);

            const MucUserSet& occupants() const { return this->_users; }

            bool isOccupant(const XMPP::Jid& user_jid) const {
                return this->users().find_jid(user_jid) != this->users().end();
            }

			void broadcastIq(Stanza* stanza, const ConstStanzaHandler& on_result = ConstStanzaHandler(),
					const TimeoutHandler& on_timeout = TimeoutHandler());

            virtual void handleStanza(Stanza* stanza) throw();

        protected:

            virtual void notifyUserStatus(
                    const Jid& jid,
                    const std::string& nick,
                    bool available) = 0;


		private:

			void handlePresence(const Stanza& stanza) throw();

			void handleGroupChat(const Stanza& stanza);

			void presentUsers(const Jid& jid);

			void addUser(const std::string& nick, const Jid& user_jid);

			void removeUser(const Jid& user_jid, const std::string& status);

			Stanza* createPresenceStanza(const MucUser& user);

			MucUserSet& users() { return this->_users; }
			const MucUserSet& users() const { return this->_users; }

			MucUserSet _users;
	};
}

#endif
