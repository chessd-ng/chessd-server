#include "Xml.hh"
#include "iksutil.hh"

#include "../Util/utils.hh"

using namespace std;
using namespace Util;

namespace XML {
	Item::~Item() { }

	Tag::Tag(const Tag& tag) : _name(tag.name()), _attributes(tag.attributes())   {
		ChildrenList::const_iterator it;
		for(it = tag.children().begin(); it != tag.children().end(); ++it) {
			this->children().push_back((*it)->clone());
		}
	}

	Tag::~Tag() {
		this->clearChildren();
	}

	void Tag::xml(string& str) const {
		str += '<';
		str += this->name();
		AttributeMap::const_iterator it;
		for(it = this->attributes().begin(); it != this->attributes().end(); ++it) {
			str += ' ';
			str += it->first;
			str += '=';
			str += '"';
			str += it->second;
			str += '"';
		}
		if(this->children().empty()) {
			str += "/>";
		} else {
			str += '>';
			ChildrenList::const_iterator it;
			for(it = this->children().begin(); it != this->children().end(); ++it)
				(*it)->xml(str);
			str += "</";
			str += this->name();
			str += ">";
		}
	}

	Item* Tag::clone() const {
		return new Tag(*this);
	}

	void Tag::swap(Tag& tag) {
		this->name().swap(tag.name());
		this->attributes().swap(tag.attributes());
		this->children().swap(tag.children());
	}

	Item* CData::clone() const {
		return new CData(*this);
	}

	void CData::xml(string& str) const {
		str += this->data();
	}

	string Tag::xml() const {
		string tmp;
		this->xml(tmp);
		return tmp;
	}

	void Tag::clearChildren() {
		foreach(child, this->children()) {
			delete *child;
		}
	}

	TagGenerator::TagGenerator() : last(0) { }

	TagGenerator::~TagGenerator() {
		while(not this->tag_stack.empty()) {
			delete this->tag_stack.top();
			this->tag_stack.pop();
		}
	}

	void TagGenerator::openTag(const string& name) {
		this->tag_stack.push(new Tag(name));
	}

	Tag* TagGenerator::closeTag() {
		Tag *tag = this->tag_stack.top();
		this->tag_stack.pop();
		if(not this->tag_stack.empty())
			this->tag_stack.top()->children().push_back(tag);
		return last=tag;
	}

	void TagGenerator::addAttribute(const string& name, const string& value) {
		this->tag_stack.top()->attributes().insert(make_pair(name, value));
	}

	void TagGenerator::addCData(const string& data) {
		this->tag_stack.top()->children().push_back(new CData(data));
	}

	void TagGenerator::addChild(Tag* tag) {
		this->tag_stack.top()->children().push_back(tag);
	}

	Tag* TagGenerator::getLastTag() {
		return this->last;
	}

	bool TagGenerator::empty() const {
		return this->tag_stack.empty();
	}

	const Tag* Tag::getChild(const std::string& name, const Tag* begin) const {
		ChildrenList::const_iterator it;
		for(it = this->children().begin();
				it != this->children().end();
				++it) {
			const Tag* tag = dynamic_cast<Tag*>(*it);
			if(tag and tag->name() == name and begin == 0)
				return tag;
			else if(tag == begin)
				begin = 0;
		}
		return 0;
	}

	Tag* Tag::getChild(const std::string& name, const Tag* begin) {
		ChildrenList::const_iterator it;
		for(it = this->children().begin();
				it != this->children().end();
				++it) {
			Tag* tag = dynamic_cast<Tag*>(*it);
			if(tag and tag->name() == name && begin == 0)
				return tag;
			else if(begin == tag)
				begin = 0;
		}
		return 0;
	}

	Description::Description() : type_count(0) { }

	Description::~Description() {
	}

	bool Description::loadFromFile(const std::string& filename) {
		Tag* xml = loadXmlFile(filename);
		if(xml == 0)
			return false;
		if(xml->name()!="xd") {
			delete xml;
			return false;
		}
		bool root_found=false;
		bool error = false;
		foreach(child, xml->children()) {
			Tag* tag = dynamic_cast<Tag*>(*child);
			if(tag) {
				if(tag->name() == "root") {
					if(root_found) {
						error = true; break;
					}
					if(not tag->hasAttribute("name")) {
						error = true; break;
					}
					this->root_name = tag->getAttribute("name");
					if(not tag->hasAttribute("type")) {
						if(not this->parseType(tag,"%root_type")) {
							error = true; break;
						}
						this->root_type="%root_type";
					} else {
						this->root_type = tag->getAttribute("type");
					}
					root_found=true;
				} else if(tag->name() == "type") {
					if(not tag->hasAttribute("name")) {
						error = true; break;
					}
					if(not this->parseType(tag, tag->getAttribute("name"))) {
						error = true; break;
					}
				} else {
					error = true; break;
				}
			}
			/* ignore char data */
		}
		delete xml;
		if(not root_found) {
			error = true;
		}
		foreach(type, this->types) foreach(child, type->second.children) {
			if(not hasKey(this->types, child->type))
				error = true;
		}
		if(not hasKey(this->types, this->root_type))
			error = true;
		return not error;
	}

	bool Description::parseType(const Tag* xml, const std::string& type_name) {
		TypeDesc type;
		bool error = false;
		foreach(child, xml->children()) {
			const Tag* tag = dynamic_cast<const Tag*>(*child);
			if(tag) {
				if(tag->name()=="attribute") {
					if(not tag->hasAttribute("name")) {
						error = true; break;
					}
					const std::string& att_name = tag->getAttribute("name");
					bool required;
					if(tag->hasAttribute("required")) {
						if(tag->getAttribute("required") == "true")
							required = true;
						else
							required = false;
					} else {
						required=true;
					}
					bool fixed;
					if(tag->hasAttribute("fixed") and tag->getAttribute("fixed") == "true")
						fixed=true;
					else
						fixed=false;
					std::string def_val;
					if(tag->hasAttribute("default")) {
						def_val=tag->getAttribute("fixed");
					}
					FieldType att_type;
					if(tag->hasAttribute("type")) {
						if(tag->getAttribute("type") == "string") {
							att_type=FieldString;
						} else if(tag->getAttribute("type") == "number") {
							att_type=FieldNumber;
						} else {
							error=true; break;
						}
					} else {
						att_type=FieldString;
					}
					type.attributes.insert(make_pair(att_name,
								AttributeDesc(att_type, required, fixed, def_val)));
				} else if(tag->name()=="cdata") {
					type.cdata=true;
				} else if(tag->name()=="child") {
					if(not tag->hasAttribute("name")) {
						error = true; break;
					}
					const string& ch_name = tag->getAttribute("name");
					int minOccur;
					if(tag->hasAttribute("min")) {
						if(not isNumber(tag->getAttribute("min"))) {
							error=true; break;
						}
						minOccur=str2int(tag->getAttribute("min"));
					} else {
						minOccur=1;
					}
					int maxOccur;
					if(tag->hasAttribute("max")) {
						if(tag->getAttribute("max") == "unbounded") {
							maxOccur=0x7fffffff; 
						} else {
							if(not isNumber(tag->getAttribute("max"))) {
								error = true; break;
							}
							maxOccur=str2int(tag->getAttribute("max"));
						}
					} else {
						maxOccur=1;
					}
					string ch_type;
					if(tag->hasAttribute("type")) {
						ch_type = tag->getAttribute("type");
					} else {
						ch_type = int2str(this->type_count++);
						if(not this->parseType(tag, ch_type)) {
							error = true; break;
						}
					}
					type.children.push_back(ChildDesc(ch_name, ch_type, minOccur, maxOccur));
				} else if(tag->name()=="any") {
					type.any = true;
				} else {
					error = true;
					break;
				}
			}
		}
		if(type.any and not type.children.empty()) {
			error = true;
		}
		if(error==true) {
			return false;
		} else {
			this->types.insert(make_pair(type_name, type));
		}
		return not error;
	}

	bool Description::validateXML(Tag* tag) {
		if(tag->name() != this->root_name)
			return false;
		return this->_validate(tag, this->types[this->root_type]);
	}

	bool Description::_validate(Tag* xml, const TypeDesc& type) {
		foreach(attribute, xml->attributes()) {
			if(not hasKey(type.attributes, attribute->first))
				return false;
		}
		foreach(att_desc, type.attributes) {
			if(not xml->hasAttribute(att_desc->first)) {
				if(att_desc->second.required)
					return false;
				else
					xml->setAttribute(att_desc->first, att_desc->second.default_value);
			} else {
				const string& val = xml->attributes()[att_desc->first];
				if(att_desc->second.fixed and val!= att_desc->second.default_value) {
					return false;
				} else {
					if(att_desc->second.type == FieldNumber) {
						if(not isNumber(val))
							return false;
					}
				}
			}
		}
		if(type.any) return true;
		ChildrenList::const_iterator child = xml->children().begin();
		int last_count = 0;
		foreach(child_desc, type.children) {
			bool done = false;
			while(not done) {
				CData* cdata;
				Tag* tag;
				if(child != xml->children().end() and (cdata=dynamic_cast<CData*>(*child))) {
					if(not type.cdata and not isSpace(cdata->data()))
						return false;
					++child;
				} else if(child != xml->children().end() and (tag=dynamic_cast<Tag*>(*child)) and tag->name()==child_desc->name) {
					++last_count;
					++child;
				} else  {
					if(last_count < child_desc->minOccur or last_count > child_desc->maxOccur)
						return false;
					last_count = 0;
					done = true;
				}
			}
		}
		if(not type.cdata) {
			int remove_count = 0;
			ChildrenList::iterator tmp = xml->children().begin();
			foreach(it, xml->children()) {
				const CData* cdata = dynamic_cast<const CData*>(*it);
				if(cdata) {
					remove_count++;
					delete *it;
				} else {
					swap(*it, *tmp);
					++tmp;
				}
			}
			xml->children().erase(xml->children().end()-remove_count, xml->children().end());
		}
		return true;
	}
	Tag* loadXmlFile(const std::string& filename) {
		return iksReadXMLFile(filename);
	}
}
