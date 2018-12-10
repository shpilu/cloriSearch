//
// IndexerManager main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_INDEXER_MANAGER_H_
#define CLORIS_INDEXER_MANAGER_H_

namespace cloris {

//
// 一个IndexerManager对象是一个conjunctions=N的集合 
//
class IndexerManager {
public:
    IndexerManager();
    ~IndexerManager();
    bool DeclareTerm(Term& term);
    bool Add(const Conjunction& conjunction, int docid);
    bool Add(const Disjunction& disjunction, int docid);
    std::vector<int> Search(Query& query, int limit);
    void GetPostingLists(Query& query, ConjunctionScorer& scorer);
private:
    std::unordered_map<std::string, Indexer*> indexer_table_;
    int conjunctions_;
};

} // namespace cloris

#endif // CLORIS_INDEXER_MANAGER_H_

