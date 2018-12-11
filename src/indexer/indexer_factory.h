//
// indexer factory class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_INDEXER_FACTORY_H_
#define CLORIS_INDEXER_FACTORY_H_

#include <string>

namespace cloris {

class Indexer;

class IndexerFactory {
public:
    static IndexerFactory* instance() noexcept;

    IndexerFactory() {}
    ~IndexerFactory() {}
    Indexer* CreateIndexer(const std::string& name, const std::string& key_type, const std::string& value_type);
};

} // namespace cloris

#endif // CLORIS_INDEXER_FACTORY_H_
