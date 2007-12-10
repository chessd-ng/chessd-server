#ifndef XMLCDATA_HH
#define XMLCDATA_HH

#include <string>
#include "../Move.hh"

namespace XML {


	class CData : public Item {
		public:
			/* default constructor */
			CData(const std::string& data = "") : _data(data) { }

			/* copy constructor */
			CData(const CData& cdata) : _data(cdata.data()) { }

			/* move constructor */
			CData(Moved<CData> cdata) { this->data().swap(cdata->data()); }

			virtual ~CData() { }

			/* return the char data */
			std::string& data() { return this->_data; }
			const std::string& data() const { return this->_data; }

			/* clone this object */
			virtual CData* clone() const;

			/* generate xml */
			virtual void xml(std::string&) const;

		private:
			std::string _data;
	};

}

#endif
