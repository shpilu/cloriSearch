//
// indexer manager core implementation
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include "internal/log.h"
#include "indexer_factory.h"
#include "indexer_manager.h"

namespace cloris {

IndexerManager::IndexerManager() {
}

IndexerManager::~IndexerManager() {
}

bool IndexerManager::DeclareTerm(const IndexSchema_Term& term) {
    if (indexer_table_.find(term.name()) == indexer_table_.end()) {
        return true;
    }
    Indexer* indexer = IndexerFactory::instance()->CreateIndexer(term.name(), term.key_type(), term.index_type());
    if (!indexer) {
        cLog(ERROR, "unsupported indexer type");
        return false;
    }
    indexer_table_.insert(std::pair<std::string, Indexer*>(term.name(), indexer));
    return true;
}

bool IndexerManager::Add(const Conjunction& conjunction, int docid) {
    return indexer_table_[conjunction.name()]->Add(conjunction.value(), docid);
}

bool IndexerManager::Add(const Disjunction& disjunction, int docid) {
    for (auto& conjunction : disjunction.conjunctions()) {
        this->Add(conjunction, docid);
    }
    return true;
}

void IndexerManager::GetPostingLists(const Query& query, ConjunctionScorer& scorer) {
    for (auto& term : query) {
        if (indexer_table_.find(term.name()) != indexer_table_.end()) {
            std::list<int>* doc_list = indexer_table_[term.name()]->GetPostingLists(term);
            if (doc_list) {
                scorer.AddPostingList(doc_list);
            }
        }
    }
}

// implementation of <<indexing boolean expression>> conjunction algorithm 
std::vector<int> IndexerManager::Search(const Query& query, int limit) {
    ConjunctionScorer scorer;
    this->GetPostingLists(query, scorer);
    return scorer.GetMatchedDocid(this->conjunctions_);
}
// std::unordered_map<std::string, Indexer*> indexer_table_;

} // namespace cloris
