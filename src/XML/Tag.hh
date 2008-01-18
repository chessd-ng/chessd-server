#ifndef XMLTAG_HH
#define XMLTAG_HH

#include <vector>
#include <map>
#include <string>
#include <boost/ptr_container/ptr_vector.hpp>
#include "Item.hh"
#include "CData.hh"

#include "Util/CastFunctor.hh"
#include "Util/View.hh"
#include "Util/utils.hh"

namespace XML {

	typedef std::map<std::string, std::string> AttributeMap;
	typedef boost::ptr_vector<Item> ItemList;

    class child_not_found : public std::runtime_error {
        public:
            explicit child_not_found(const std::string& msg) : runtime_error(msg) { }
    };

    class attribute_not_found : public std::runtime_error {
        public:
            explicit attribute_not_found(const std::string& msg) : runtime_error(msg) { }
    };

	class Tag : public Item {
		public:
			/* default constructor */
			Tag(const std::string& name) :
				_name(name),
				_tag_view(this->children()),
				_cdata_view(this->children()) { }

			/* copy contructor */
			Tag(const Tag&);

			/* move contructor */
			Tag(Moved<Tag> tag);

			/* destructor */
			virtual ~Tag() { }
			
			/* return its name */
			std::string& name() { return this->_name; }
			const std::string& name() const { return this->_name; }

			/* return tag attributes */
			AttributeMap& attributes() { return this->_attributes; }
			const AttributeMap& attributes() const { return this->_attributes; }

			/* clone the tag */
			virtual Tag* clone() const;

			/* swap content */
			virtual void swap(Tag& tag);

			/* generate xml string */
			std::string xml() const;

			bool hasAttribute(const std::string& name) const {
				return this->attributes().find(name) != this->attributes().end();
			}

			void setAttribute(const std::string& name, const std::string& value) {
				this->attributes().insert(std::pair<std::string, std::string>(name, value));
			}

			const std::string& getAttribute(const std::string& name) const {
                AttributeMap::const_iterator iterator = this->attributes().find(name);
                if(iterator == this->attributes().end())
                    throw(attribute_not_found("Attribute not found"));
                else
                    return this->attributes().find(name)->second;
            }

			std::string& getAttribute(const std::string& name) {
				return this->attributes().find(name)->second;
			}

			Tag& findChild(const std::string& name) {
                foreach(tag, this->tags()) {
                    if(tag->name() == name)
                        return *tag;
                }
                throw (child_not_found("Child not found"));
            }

			const Tag& findChild(const std::string& name) const {
                foreach(tag, this->tags()) {
                    if(tag->name() == name)
                        return *tag;
                }
                throw (child_not_found("Child not found"));
            }

			/* generate xml string */
			virtual void xml(std::string&) const;

			void clearChildren();
		private:


			struct is_tag : public std::unary_function<Item&, bool> {
				bool operator()(const Item& item) const {
					return typeid(item) == typeid(Tag);
				}
			};

			struct is_cdata : public std::unary_function<Item&, bool> {
				bool operator()(const Item& item) const {
					return typeid(item) == typeid(CData);
				}
			};

			typedef CastFunctor<Tag, Item> cast_tag;
			typedef CastFunctor<CData, Item> cast_cdata;


			typedef View<ItemList, is_tag, cast_tag> TagView;
			typedef View<ItemList, is_cdata, cast_cdata> CDataView;

			/* Attributes */
			std::string _name;
			AttributeMap _attributes;
			ItemList _children;
			TagView _tag_view;
			CDataView _cdata_view;
		public:
			const TagView& tags() const { return _tag_view; }
			TagView& tags() { return _tag_view; }

			const CDataView& cdatas() const { return _cdata_view; }
			CDataView& cdatas() { return _cdata_view; }

			ItemList& children() { return this->_children; }
			const ItemList& children() const { return this->_children; }
	};

}

#endif
