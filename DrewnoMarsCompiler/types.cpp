#include <list>
#include <sstream>

#include "types.hpp"
#include "ast.hpp"

namespace drewno_mars{

std::string BasicType::getString() const{
	std::string res = "";
	switch(myBaseType){
	case BaseType::INT:
		res += "int";
		break;
	case BaseType::BOOL:
		res += "bool";
		break;
	case BaseType::VOID:
		res += "void";
		break;
	case BaseType::STRING:
		res += "string";
		break;
	}
	return res;
}

const DataType * BoolTypeNode::getType() const {
	return BasicType::BOOL();
}

const DataType * IntTypeNode::getType() const {
	return BasicType::INT();
}

static bool typelistMatch(const std::list<const DataType *> * first, const std::list<const DataType *> * second){
	if (first->size() != second->size()){
		return false;
	}

	auto kTypeItr = first->begin();
	auto cTypeItr = second->begin();
	while (kTypeItr != first->end()){
		auto kType = *kTypeItr;
		auto cType = *cTypeItr;
		if (kType != cType){
			return false;
		}
		++kTypeItr;
		++cTypeItr;
	}
	return true;
}

TypeList * TypeList::produce(const std::list<TypeNode *> * typeNodes){
	//Use a flyweight here
	static std::list<TypeList *> knownLists;

	std::list<const DataType *> * candidate = new std::list<const DataType *>();
	for (auto node : *typeNodes){
		const TypeNode * n = &(*node);
		const DataType * t = n->getType();
		candidate->push_back(t);
	}

	TypeList * exists = nullptr;
	for (TypeList * known : knownLists){
		if (typelistMatch(known->types, candidate)){
			exists = known;
			break;
		}
	}

	if (!exists){
		TypeList * t = new TypeList(candidate);
		knownLists.push_back(t);
		return t;
	} else {
		return exists;
	}
}

} //End namespace
