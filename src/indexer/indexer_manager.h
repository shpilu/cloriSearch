//
// IndexerManager main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_INDEXER_MANAGER_H_
#define CLORIS_INDEXER_MANAGER_H_

#include <unordered_map>
#include "index_schema.pb.h"
#include "inverted_index.pb.h"
#include "indexer/conjunction_scorer.h"
#include "indexer.h"
#include "query.h"

namespace cloris {

// 一个IndexerManager对象是一个conjunctions=N的集合 
class IndexerManager {
public:
    IndexerManager(size_t conj);
    ~IndexerManager();
    bool DeclareTerm(const IndexSchema_Term& term);
    bool Add(const Conjunction& conjunction, int docid);
    bool Add(const Disjunction& disjunction, int docid);
    std::vector<int> Search(const Query& query, int limit);
    void GetPostingLists(const Query& query, ConjunctionScorer& scorer);
private:
    InvertedList zlist_; // special Zero_list for Zero-index
    std::unordered_map<std::string, Indexer*> indexer_table_;
    size_t conjunctions_;
};

} // namespace cloris

#endif // CLORIS_INDEXER_MANAGER_H_

