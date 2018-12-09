//
// simple indexer implementation 
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
#include "simple_indexer.h"

namespace cloris {

SimpleIndexer::SimpleIndexer() :
   name_(name),
   key_type_(key_type),
   index_type_(index_type) {
}

SimpleIndexer::~SimpleIndexer() {
}

// std::unordered_map<T, ItemListHead> lists_;
bool SimpleIndexer::Add(AttributeValue& value, int docid) {

}

} // namespace cloris
