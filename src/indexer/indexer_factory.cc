//
// indexer factory class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
#include "indexer_factory.h"

namespace cloris {

IndexerFactory* IndexerFactory::instance() {
    return singleton<IndexerFactory>();
}

Indexer* CreateIndexer(const std::string& name, const std::string& key_type, const std::string& index_type) {
    if (type == "default") {
        if (index_type == "int") {
            returrn new SimpleIndexer<int>();
        } else if (index_type == "string") {
            return new SimpleIndexer<std::string>();
        }
    } else {
        return NULL;
    }
}

} // namespace cloris
