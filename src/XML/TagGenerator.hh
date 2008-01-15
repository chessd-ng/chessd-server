#ifndef XMLTAGGENERATOR_HH
#define XMLTAGGENERATOR_HH

#include <string>
#include <stack>
#include "Tag.hh"
#include "CData.hh"

namespace XML {

	class TagGenerator {
		public:
			TagGenerator();
			~TagGenerator();

			void openTag(const std::string& name);
			Tag* closeTag();

			void addAttribute(const std::string& name, const std::string& value);

			void addCData(const std::string& data);

			void addChild(Tag* tag);

			Tag* getTag();

			bool empty() const;

		private:
			std::stack<Tag*> tag_stack;
			Tag* tag;
	};
}

#endif
