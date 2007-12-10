#ifndef XMLDESCRIPTION_HH
#define XMLDESCRIPTION_HH

#include "Tag.hh"
#include "CData.hh"
#include <string>

namespace XML {

	/* Loads a xml file */
	Tag* loadXmlFile(const std::string& filename);

	class Description {
		public:
			Description();
			~Description();

			/* TODO: remove this method, make it a constructor
			 * throw an exception on error */
			bool loadFromFile(const std::string& desc_filename);
			bool validateXML(Tag& xml);

		private:
			enum FieldType {
				FieldString,
				FieldNumber
			};
			struct AttributeDesc {
				FieldType type;
				bool required;
				bool fixed;
				/* the fixed value */
				std::string default_value;
				AttributeDesc(FieldType type, bool required, bool fixed, const std::string& default_value) :
					type(type),
					required(required),
					fixed(fixed),
					default_value(default_value) { }
			};
			struct ChildDesc {
				std::string name;
				std::string type;
				int minOccur, maxOccur;
				ChildDesc(const std::string& name, const std::string& type, int minOccur, int maxOccur) :
					name(name),
					type(type),
					minOccur(minOccur),
					maxOccur(maxOccur) { }
			};
			struct TypeDesc {
				std::map<std::string, AttributeDesc> attributes;
				std::vector<ChildDesc> children;
				bool cdata; /* does it allow cdata? */
				bool any_children; /* the children are unspecified */
				bool any_attributes; /* the attributes are unspecified */
				TypeDesc() : cdata(false), any_children(false), any_attributes(false) { }
			};
			std::string root_type, root_name;
			std::map<std::string, TypeDesc> types;
			int type_count;
			/* Auxilary functions */
			bool parseType(const Tag& xml, const std::string& name);
			bool _validate(Tag& xml, const TypeDesc& type);
	};
}

#endif
