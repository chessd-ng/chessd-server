#ifndef UTILIDENTIFIER_HH

/* This class is a translator type.
 * It translate any constant type, that
 * provide an appropriate strict
 * less operator, to
 * an int, so that you can use that
 * int as a reprentation of the
 * original type */

#include "Sdb.hh"

namespace Util {

	template <class Type>
		class Identifier {
			public:
				Identifier() : next_id(0) {
					/* init db indices */
					this->id_index = db.createIndex<KeyMember(&value::first), false>();
					this->type_index = db.createIndex<KeyMember(&value::second), false>();
				}
				~Identifier() {
				}

				const Type& getOriginal(unsigned int id) const {
					return *this->id_index.find(id)->second->second;
				}
				/* TODO allow the removal of an index and do a correct check
				 * for the available index */
				unsigned int getID(const Type& t) const {
					TypeIndex::iterator it = this->type_index.find(t);
					if(it == this->type_index.end()) {
						this->db.insert(value(next_id++, t));
						return this->type_index.find(t)->second->first;
					} else {
						return tmp->second->first;
					}
				}
			private:

				/* db to store ID's */
				typedef std::pair<unsigned int, Type> value;
				mutable SimpleDatabase<value> db;

				/* database indices */
				typedef SDBIndex<value, KeyMember(&value::first), false>* IDIndex;
				typedef SDBIndex<value, KeyMember(&value::second), false>* TypeIndex;

				IDIndex id_index;
				TypeIndex type_index;

				unsigned int next_id;
		};

}

#endif
