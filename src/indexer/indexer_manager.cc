//
// indexer manager core implementation
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
#include "index_manager.h"

namespace cloris {

IndexerManager::IndexerManager() {
}

IndexerManager::~IndexerManager() {
}

bool IndexerManager::DeclareTerm(Term& term) {
    if (indexer_table_.find(term.name()) == indexer_table_.end()) {
        return true;
    }
    Indexer* indexer = IndexerFactory::instance()->CreateIndexer(term.name(), term.key_type(), tem.index_type());
    if (!indexer) {
        cLog(ERROR, "unsupported indexer type");
        return false;
    }
    indexer_table_.insert(term.name(), indexer);
    return true;
}

bool IndexerManager::Add(const Conjunction& conjunction, int docid) {
    indexer_table_[conjunction.name()]->Add(conjunction.value(), docid);
}

bool IndexerManager::Add(const Disjunction& disjunction, int docid) {
    for (auto& conjunction : disjunction.conjunctions()) {
        this->Add(conjunction, docid)
    }
    return true;
}
    void AddPostingList(std::list<int>* doc_list);

void IndexerManager::GetPostingLists(Query& query, ConjunctionScorer& scorer) {
    for (auto& term : query) {
        if (indexer_table_.find(term.first) != indexer_table_.end()) {
            std::list<int>* doc_list = indexer_table_[term.first]->GetPostingLists(term.second);
            if (doc_list) {
                scorer.AddPostingList(doc_list);
            }
        }
    }
}

// implementation of <<indexing boolean expression>> conjunction algorithm 
std::vector<int> IndexerManager::Search(Query& query, int limit) {
    ConjunctionScorer scorer;
    this->GetPostingLists(query, scorer);
    return scorer.GetMatchedDocid(this->conjunctions_);
}
// std::unordered_map<std::string, Indexer*> indexer_table_;

} // namespace cloris
