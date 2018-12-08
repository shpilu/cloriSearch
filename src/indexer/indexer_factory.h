//
// indexer factory class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_INDEXER_FACTORY_H_
#define CLORIS_INDEXER_FACTORY_H_

namespace cloris {

class IndexerFactory {
public:
    static IndexerFactory* instance();
    Indexer* CreateIndexer(const std::string& type);
};

// Indexer* indexer = IndexerFactory::instance()->CreateIndexer(term.type());

} // namespace cloris

#endif // CLORIS_INDEXER_FACTORY_H_

IndexerFactory* IndexerFactory::instance() {
    return singleton<IndexerFactory>();
}

Indexer* CreateIndexer(const std::string& type, const std::string& value_type) {
    if (type == "default") {
        if (value_type == "int") {
            returrn new SimpleIndexer<int>();
        } else if (value_type == "string") {
            return new SimpleIndexer<std::string>();
        }
    } else {
        return NULL;
    }
}

