//
// inverted index core implementation
// InvertedIndex is the manager class of inverted index
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include "internal/log.h"
#include "indexer/indexer_manager.h"
#include "inverted_index.h"

namespace cloris {

static size_t get_dnf_size(const Disjunction& disjunction) {
    size_t dnf_size = 0;
    for (auto &conj : disjunction.conjunctions()) {
        // Belong To
        if (!conj.has_bt() || conj.bt()) {
            ++dnf_size;
        }
    }
    return dnf_size;
}

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
        if (tmp_set.find(term.name()) == tmp_set.end()) {
            for (size_t i = 0; i < table_size; ++i) {
                if (is_first_loop) {
                    
                    new(&itable_[i]) IndexerManager(i);
                }
                cLog(DEBUG, "declare term: %s, index=%d", term.name().c_str(), i);
                itable_[i].DeclareTerm(term);
            }
            tmp_set.insert(term.name());
        }
        is_first_loop = false;
    }
    return true;
}

//
// DNF: (A ^ B ^ C) v (A ^ D), the first step is to cut this expression into
// A ^ B ^ C and A ^ D, then add them into the inverted list one by one 
bool InvertedIndex::Add(const DNF& dnf, bool is_incremental) {
    for (auto& disjunction : dnf.disjunctions()) {
        this->Add(disjunction, dnf.docid(), is_incremental);
    } 
    return true;
}

// deal with city, device...
bool InvertedIndex::Add(const Disjunction& disjunction, int docid, bool is_incremental) {
    size_t conj_size = get_dnf_size(disjunction);
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

// TODO
void InvertedIndex::GetStandardQuery(const Query& query, Query& std_query) {
    for (auto &p : query) {
        if (terms_.find(p.name()) != terms_.end()) {
            std_query.Append(p);
        }
    }
    return ;
}

// TODO
std::vector<int> InvertedIndex::Search(const Query& query, int limit) {
    std::vector<int> response;
    Query std_query;
    // clean unexisted key
    this->GetStandardQuery(query, std_query);
    for (int i = static_cast<int>(std_query.size()); i >= 0; --i) {
        std::vector<int> tmp_vec = itable_[i].Search(std_query, limit);
        for (auto &p : tmp_vec) {
            response.push_back(p);
        }
    }
    return response;
}

} // namespace cloris
