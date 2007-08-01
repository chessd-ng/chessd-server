#ifndef XML_HH
#define XML_HH

#include <vector>
#include <map>
#include <string>
#include <stack>
#include "utils.hh"

namespace XML {

	class Item {
		public:
			virtual ~Item();
			virtual Item* clone() const = 0;
			virtual void xml(std::string&) const = 0;
	};

	typedef std::map<std::string, std::string> AttributeMap;
	typedef std::vector<Item*> ChildrenList;

	class Tag : public Item {
		public:
			/* default constructor */
			Tag(const std::string& name) : _name(name) { }

			/* copy contructor */
			Tag(const Tag&);

			/* destructor */
			virtual ~Tag();
			
			/* return its name */
			std::string& name() { return this->_name; }
			const std::string& name() const { return this->_name; }

			/* return tag attributes */
			AttributeMap& attributes() { return this->_attributes; }
			const AttributeMap& attributes() const { return this->_attributes; }

			/* return tag children */
			ChildrenList& children() { return this->_children; }
			const ChildrenList& children() const { return this->_children; }

			/* clone the tag */
			virtual Item* clone() const;

			/* swap data */
			virtual void swap(Tag& tag);

			/* generate xml doc */
			virtual void xml(std::string&) const;

			/* generate xml doc */
			std::string xml() const;

			/* clear children */
			void clearChildren();

			bool hasAttribute(const std::string& name) const { return hasKey(this->attributes(), name); }

			const std::string& getAttribute(const std::string& name) const;
			std::string& getAttribute(const std::string& name);

			void setAttribute(const std::string& name, const std::string& value) {
				this->attributes().insert(std::pair<std::string, std::string>(name, value));
			}

			const Tag* getChild(const std::string& name, const Tag* begin = 0) const;
			Tag* getChild(const std::string& name, const Tag* begin = 0);

		private:
			std::string _name;
			AttributeMap _attributes;
			ChildrenList _children;
	};

	class CData : public Item {
		public:
			/* defalut constructor */
			CData(const std::string& data = "") : _data(data) { }

			/* copy constructor */
			CData(const CData& cdata) : _data(cdata.data()) { }

			/* return its char data */
			std::string& data() { return this->_data; }
			const std::string& data() const { return this->_data; }

			/* clone the cdata */
			virtual Item* clone() const;

			/* generator xml doc */
			virtual void xml(std::string&) const;

		private:
			std::string _data;
	};

	class TagGenerator {
		public:
			TagGenerator();
			~TagGenerator();

			void openTag(const std::string& name);
			Tag* closeTag();

			void addAttribute(const std::string& name, const std::string& value);

			void addCData(const std::string& data);

			void addChild(Tag* tag);

			Tag* getLastTag();

		private:
			std::stack<Tag*> tag_stack;
			Tag* last;
	};

	class Description {
		public:
			Description();
			~Description();

			bool loadDescription(const std::string& desc_filename);
			bool validateXML(Tag* xml);
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
			};
			std::string root_type, root_name;
			std::map<std::string, TypeDesc> types;
			int type_count;
			/* Auxilary functions */
			bool parseType(const Tag* xml, const std::string& name);
			bool _validate(Tag* xml, const TypeDesc& type);
	};
}

#endif
