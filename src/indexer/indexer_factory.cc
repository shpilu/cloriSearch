//
// indexer factory class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include "internal/singleton.h"
#include "simple_indexer.h"
#include "indexer_factory.h"

namespace cloris {

IndexerFactory* IndexerFactory::instance() noexcept {
    return Singleton<IndexerFactory>::instance();
}

Indexer* IndexerFactory::CreateIndexer(const std::string& name, const std::string& key_type, const std::string& index_type) {
    if (index_type == "simple") {
        if (key_type == "int32") {
            return new SimpleIndexer(name, ValueType::INT32);
        } else if (key_type == "string") {
            return new SimpleIndexer(name, ValueType::STRING);
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

} // namespace cloris
