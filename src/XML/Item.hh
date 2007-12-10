#ifndef XMLITEM_HH
#define XMLITEM_HH

#include <string>

namespace XML {

	class Item {
		public:
			virtual ~Item() { }
			virtual Item* clone() const = 0;
			virtual void xml(std::string&) const = 0;
	};
}


#endif
