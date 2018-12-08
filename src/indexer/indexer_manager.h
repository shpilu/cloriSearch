//
// IndexerManager main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_INDEXER_MANAGER_H_
#define CLORIS_INDEXER_MANAGER_H_

namespace cloris {

class IndexerManager {
public:
    IndexerManager();
    ~IndexerManager();
    bool AddTerm(Term& term);
    bool Add(const Disjunction& disjunc, int docid);
    std::vector<int> Search(Query& query, int limit);
private:
    std::unordered_map<std::string, Indexer*> indexer_table_;
};

typedef std::unordered_map<std::string, IndexerManager*> IndexerTable;

} // namespace cloris

#endif // CLORIS_INDEXER_MANAGER_H_

