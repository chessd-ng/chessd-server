#include "xml.hh"
#include "iksutil.hh"
#include "utils.hh"

using namespace std;

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
			str += it->second;
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
		ChildrenList::const_iterator it;
		for(it = this->children().begin(); it != this->children().end(); ++it) {
			delete *it;
		}
	}

	TagGenerator::TagGenerator() : last(0) { }
	TagGenerator::~TagGenerator() {
		while(not this->tag_stack.empty()) {
			delete tag_stack.top();
			tag_stack.pop();
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

	const std::string& Tag::getAttribute(const std::string& name) const {
		return this->attributes().find(name)->second;
	}

	std::string& Tag::getAttribute(const std::string& name) {
		return this->attributes().find(name)->second;
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
		return NULL;
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
		return NULL;
	}

	Description::Description() : type_count(0) { }

	Description::~Description() { }

	bool Description::loadDescription(const std::string& filename) {
		Tag* xml = iksReadXML(filename);
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
			const Tag* tag = dynamic_cast<const Tag*>(xml);
			if(tag) {
				if(tag->name()=="attribute") {
					if(not tag->hasAttribute("name")) {
						error = true; break;
					}
					const std::string& att_name = tag->getAttribute("name");
					bool required;
					if(tag->hasAttribute("required") and tag->getAttribute("required") == "true")
						required=true;
					else
						required=false;
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
								error=true; break;
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
				} else {
					error = true;
					break;
				}
			}
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

	bool Description::_validate(Tag* tag, const TypeDesc& type) {
		foreach(attribute, tag->attributes()) {
			if(not hasKey(type.attributes, attribute->first))
				return false;
		}
		foreach(att_desc, type.attributes) {
			if(not tag->hasAttribute(att_desc->first)) {
				if(att_desc->second.required)
					return false;
				else
					tag->setAttribute(att_desc->first, att_desc->second.default_value);
			} else {
				const string& val = tag->attributes()[att_desc->first];
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
		vector<ChildDesc>::const_iterator child_desc = type.children.begin();
		const string* last_child = NULL;
		int last_count = -1;
		/* FIXME when minOccur = 0 */
		foreach(it, tag->children()) {
			Tag* child = dynamic_cast<Tag*>(*it);
			if(child) {
				if(child_desc == type.children.end())
					return false;
				if(not this->_validate(child, this->types[child_desc->type]))
					return false;
				if(last_count == -1) {
					last_count = 1;
					last_child = &child->name();
				} else if(*last_child == child->name()) {
					last_count++;
				} else {
					if(*last_child != child_desc->name
							or last_count < child_desc->minOccur
							or last_count > child_desc->maxOccur)
						return false;
					last_count = 1;
					last_child = &child->name();
					++child_desc;
					if(child_desc == type.children.end())
						return false;
				}
			} else {
				const CData* cdata = dynamic_cast<const CData*>(*it);
				if(not isSpace(cdata->data()) and not type.cdata)
					return false;
			}
		}
		if(last_count > 0) {
			if(*last_child != child_desc->name
					or last_count < child_desc->minOccur
					or last_count > child_desc->maxOccur)
				return false;
		}
		if(not type.cdata) {
			int remove_count = 0;
			ChildrenList::iterator tmp = tag->children().begin();
			foreach(it, tag->children()) {
				const CData* cdata = dynamic_cast<const CData*>(*it);
				if(cdata) {
					remove_count++;
				} else {
					swap(*it, *tmp);
					++tmp;
				}
			}
			tag->children().erase(tag->children().end()-remove_count, tag->children().end());
		}
		return true;
	}
}
