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
    Indexer* CreateIndexer(const std::string& name, const std::string& key_type, const std::string& value_type);
};

// Indexer* indexer = IndexerFactory::instance()->CreateIndexer(term.type());

} // namespace cloris

#endif // CLORIS_INDEXER_FACTORY_H_
