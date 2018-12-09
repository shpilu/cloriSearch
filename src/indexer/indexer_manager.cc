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

bool IndexerManager::Add(const Disjunction& disjunc, int docid) {
    for (auto& p : disjunc.attribute()) {
        indexer_table_[p.name()]->Add(p.value());
    }
    return true;
}

// lucene implementation
// Conjunction Algorithm refered from << indexing boolean expression >>
// 每个倒排链起名叫posting list
class ConjunctionScorer {
public:
    std::vector<int> GetMatchedDocid(int k);
    std::vector<PostingList> plists_;
}

std::vector<int> ConjunctionScorer::GetMatchedDocid(int k) {
    std::vector<int> ret;
    if (k == 0) {
        k = 1;
    }
    if (plists_.size() < k) {
        return ret;
    }
    int next_id;
    while (plists_[k - 1].current_entry() != PostingList::EOL) {
        sort(plists_begin(), plists_.end());
    } 
    if (plists_[0].current_entry() == plists_[k - 1].current_entry()) {
        // TODO support NOT IN
        next_id = plists_[k - 1].current_entry() + 1;
        if (false) {
        } else {
            ret.push_back(plists_[k - 1].current_entry());
            // K=2, 2,2,2,2
            for (int L = k; L < plists_.size(); ++L) {
                if (plists_[L] < next_id) {
                    plists_[L].SkipTo(next_id);
                } else {
                    break;
                }
            }
        }
    } else {
        next_id = plists_[k - 1].current_entry();
    }
    for (int L = 0; L < k; ++L) {
        plists_[L].SkipTo(next_id);
    }
}

void IndexerManager::GetPostingLists(Query& query, ConjunctionScorer& scorer) {
    for (auto& p : query) {
        if (indexer_table_.find(p.name()) != indexer_table_.end()) {
            scorer.plists_.push_back(indexer_table_[p.name()]);
        }
    }
}

// implementation of <<indexing boolean expression>> conjunction algorithm 
std::vector<int> IndexerManager::Search(Query& query, int limit) {
    ConjunctionScorer scorer;
    this->GetPostingLists(query, scorer);
    return scorer.GetMatchedDocid(this->seq);
}
// std::unordered_map<std::string, Indexer*> indexer_table_;


} // namespace cloris
