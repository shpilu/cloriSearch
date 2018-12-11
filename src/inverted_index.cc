//
// inverted index core implementation
// InvertedIndex is the manager class of inverted index
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include "indexer/indexer_manager.h"
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

    size_t table_size = term_size + 1;

    itable_ = static_cast<IndexerManager*>(malloc(sizeof(IndexerManager) * table_size));
    memset(itable_, 0, sizeof(IndexerManager) * table_size);
    if (!itable_) {
        err_msg = "itable memory allocation failed";
        return false;
    }

    std::set<std::string> tmp_set;
    // TODO use a better implementation
    bool is_first_loop = true;
    for (auto& term : schema.terms()) {
        if (tmp_set.find(term.name()) != tmp_set.end()) {
            for (size_t i = 0; i < table_size; ++i) {
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

bool InvertedIndex::Add(const DNF& dnf, bool is_incremental) {
    for (auto& disjunction : dnf.disjunctions()) {
        this->Add(disjunction, dnf.docid(), is_incremental);
    } 
    return true;
}

// deal with city, device...
bool InvertedIndex::Add(const Disjunction& disjunction, int docid, bool is_incremental) {
    size_t conj_size = disjunction.conjunctions().size();
    IndexerManager& manager = itable_[conj_size];
    manager.Add(disjunction, docid);
    return true;
}

// TODO
bool InvertedIndex::Update(DNF *dnf, int docid) {
    return true;
}

// TODO
bool InvertedIndex::Del(int docid) {
    return true;
}

void InvertedIndex::GetStandardQuery(const Query& query) {
    return ;
}

// TODO
std::vector<int> InvertedIndex::Search(const Query& query, int limit) {
    std::vector<int> response;
    // clean unexisted key
    this->GetStandardQuery(query);
    for (size_t i = 0; i <= query.size(); ++i) {
        std::vector<int> tmp_vec = itable_[i].Search(query, limit);
        for (auto &p : tmp_vec) {
            response.push_back(p);
        }
    }
    return response;
}

} // namespace cloris
