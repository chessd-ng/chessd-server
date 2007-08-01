#ifndef DATASTRUCTPROPERTY_HH
#define DATASTRUCTPROPERTY_HH

/* this file implements some meta tools
 * for meta lists and meta properties
 * manipuilation                            */

namespace DataStruct {

	/* meta boolean types */
	struct TrueType {};

	struct FalseType {};

	/* translates between bool and meta bools */
	template <class BooleanType> struct TranslateBooleanType;
	
	template <> struct TranslateBooleanType<TrueType> {
		static const bool value = true;
	};

	template <> struct TranslateBooleanType<FalseType> {
		static const bool value = false;
	};

	template <bool value> struct TranslateToBooleanType;

	template <> struct TranslateToBooleanType<true> {
		typedef TrueType type;
	};
	
	template <> struct TranslateToBooleanType<false> {
		typedef FalseType type;
	};
	
	/* Defines a meta property */
	template <class PropName, class PropValue> struct Property {
		typedef PropName name;
		typedef PropValue value;
	};

	/* Boolean meta property */
	template <class PropName> struct BooleanProperty : Property<PropName, TrueType> { };

	/* Constant */
	template <class Type, Type Value> struct ConstantValue {
		static const Type value = Value;
	};
	
	/* Meta list */
	template <class Type, class NextNode> struct TypeList {
		typedef Type type;
		typedef NextNode next;
	};

	/* Compares types */
	template <class T1, class T2> struct CompareTypes;

	template <class T> struct CompareTypes<T,T> {
		static const bool value = true;
	};

	template <class T1,class T2> struct CompareTypes {
		static const bool value = false;
	};
	
	/* Compare properties */
	template <class Prop1, class Prop2> struct CompareProp {	
		static const bool value = CompareTypes<typename Prop1::name, typename Prop2::name>::value and  CompareTypes<typename Prop1::value, typename Prop2::value>::value;
	};
	
	/* Search for a position in a list */
	template <class TypeList, int pos> struct GetNode;

	template <> struct GetNode<void,-1> {
		typedef void type;
	};

	template <> struct GetNode<void,0> {
		typedef void type;
	};

	template <int pos> struct GetNode<void, pos> {
		typedef void type;
	};

	template <class TypeList> struct GetNode<TypeList, 0> {
		typedef typename TypeList::type type;
	};

	template <class TypeList, int pos> struct GetNode {
		typedef typename GetNode<typename TypeList::next, pos-1>::type type;
	};

	/* Search for a type in meta list */
	template <class List, class Type> struct SearchTypeList;

	template <class Type> struct SearchTypeList<void, Type> {
		static const bool value = false;
		static const int position = -1;
	};

	template <class List, class Type> struct SearchTypeList<TypeList<Type,List>,Type> {
		static const bool value = true;
		static const int position = 0;
	};
	
	template <class List, class Type> struct SearchTypeList {
		static const bool value = SearchTypeList<typename List::next, Type>::value;
		static const int position = (value)?SearchTypeList<typename List::next, Type>::position+1:-1;
	};

	/* Search for a type in meta list, causes an error if not found */
	template <class List, class Type> struct SearchTypeListEx;

	template <class Type> struct SearchTypeListEx<void, Type> {
		// Erro: End of list, item was no found!
		typedef typename SearchTypeListEx::Item_was_not_found_in_list type; 
	};

	template <class List, class Type> struct SearchTypeListEx<TypeList<Type,List>,Type> {
		typedef Type type;
	};
	
	template <class List, class Type> struct SearchTypeListEx {
		typedef typename SearchTypeListEx<typename List::next, Type>::type type;
	};

	/* Construct a list without the values of the properties */
	template <class PropList> struct PropertyNameAdaptor;

	template <> struct PropertyNameAdaptor<void> {
		typedef void type;
	};

	template <class PropList> struct PropertyNameAdaptor {
		typedef TypeList<typename PropList::type::name,typename PropertyNameAdaptor<typename PropList::next>::type> type;
	};

	/* Search in a meta list of properties for a giben property name */
	template <class PropList, class Name> struct SearchPropName {
		static const bool value = SearchTypeList<typename PropertyNameAdaptor<PropList>::type,Name>::value;
		static const int  position   = SearchTypeList<typename PropertyNameAdaptor<PropList>::type,Name>::position;
		typedef typename GetNode<PropList,position>::type type;
	};

	/* Check if the first list has only properties allowed */
	template <class PropList, class AllowedNameList> struct ValidateList;

	template <class NameList> struct ValidateList<void, NameList> {
		static const bool value = true;
	};

	template <class PropList,class NameList> struct ValidateList {
		static const bool value = SearchTypeList<NameList, typename PropList::type::name>::value and ValidateList<typename PropList::next, NameList>::value;
	};

	/* Check for a boolean expression, cause an error if false */
	template <bool exp> struct ValidateExpression { };

	template <> struct ValidateExpression<true> {
		static void Valid() { }
	};
	
};

#endif
