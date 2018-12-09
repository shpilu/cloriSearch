//
// inverted index core implementation
// InvertedIndex is the manager class of inverted index
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include "inverted_index.h"

namespace cloris {

InvertedIndex::InvertedIndex() {
}

InvertedIndex::~InvertedIndex() {
}

bool InvertedIndex::Init(const IndexSchema& schema, std::string& err_msg) {
    size_t term_size(0);        
    for (auto& term : schema.terms()) {
        if (terms_.find(term.name()) == terms_.end()) {
            terms_.insert(term.name());
            ++term_size;
        }
    }

    if (term_size == 0) {
        err_msg = "term's size is zero";
        return false;
    }
    itable_ = malloc(sizeof(IndexerManager) * term_size);
    memset(itable_, 0, sizeof(IndexerManager) * term_size);
    if (!itable_) {
        err_msg = "itable memory allocation failed";
        return false;
    }

    std::set<std::string> tmp_set;
    // TODO use a better implementation
    bool is_first_loop = true;
    for (auto& term : schema.terms()) {
        if (tmp_set.find(term.name()) != tmp_set.end()) {
            for (int i = 0; i < term_size; ++i) {
                if (is_first_loop) {
                    new(&itable_[i]) IndexerManager();
                }
                itable_[i].DeclareTerm(term);
            }
            tmp_set.insert(term.name());
        }
        is_first_loop = false;
    }
    return true;
}

bool InvertedIndex::Add(const DNF& dnf, int docid, bool is_incremental) {
    for (auto& disjunc : dnf.disjunction()) {
        this->Add(disjunc, docid, is_incremental);
    } 
    return true;
}

// treat as a standard Disjunction now
bool InvertedIndex::Add(const Disjunction& disjunc, int docid, bool is_incremental) {
    size_t conj = disjunc.attribute().size();
    IndexerManager& manager = itable_[conj];
    manager->Add(disjunc, docid);
}

// TODO
bool InvertedIndex::Update(DNF *dnf, int docid) {
    return true;
}

// TODO
bool InvertedIndex::Del(int docid) {
}

// TODO
std::vector<int> InvertedIndex::Search(Query& query, int limit) {
    std::vector<int> response;
    // clean unexisted key
    this->GetStandardQuery(query);
    for (int i = 0; i < query.size(); ++i) {
        response += itable_->Search(query, limit);
    }
    return response;
}

} // namespace cloris
