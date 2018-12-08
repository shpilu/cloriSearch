//
// inverted index core implementation
// InvertedIndex is the manager class of inverted index
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include "inverted_index.h"

InvertedIndex::InvertedIndex() {
}

InvertedIndex::~InvertedIndex() {
}

bool InvertedIndex::Init(const IndexSchema& schema, std::string& err_msg) {
    size_t schema_size(0);        
    for (auto& term : schema.meta()) {
        if (schemas_.find(term.name()) == schemas_.end()) {
            schemas_.insert(term.name());
            ++schema_size;
        }
    }

    if (schema_size == 0) {
        err_msg = "schema's size is zero";
        return false;
    }
    itable_ = malloc(sizeof(IndexerManager) * schema_size);
    memset(itable_, 0, sizeof(IndexerManager) * schema_size);
    if (!itable_) {
        err_msg = "itable memory malloc failed";
        return false;
    }
    std::set<std::string> tmp_set;
    for (auto& term : schema.meta()) {
        if (tmp_set.find(term.name()) != tmp_set.end()) {
            for (int i = 0; i < schema_size; ++i) {
                if (!itable_[i].second) {
                    itable_[i].second = new IndexerManager();
                }
                itable_[i].second->AddTerm(term);
            }
            tmp_set.insert(term.name());
        }
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
std::vector<int> InvertedIndex::Search(Query& query, int limit) {
    std::vector<int> response;
    // clean unexisted key
    this->GetStandardQuery(query);
    for (int i = 0; i < query.size(); ++i) {
        response += itable_->Search(query, limit);
    }
    return response;
}

class InvertedIndex {
public:
    InvertedIndex();
    ~InvertedIndex();

    bool Init(IndexSchema* schema);
    Build();
    bool Add(DNF *dnf, int docid, bool is_incremental = false);
    bool Update(DNF *dnf, int docid);
    bool Del(int docid);
    std::vector<int> Search(ssmap ss, int limit);
private:
    std::set<std::string> schemas_; // age, sex, city...
    int max_conj_;
    // 10 mean the max -- is 10
    IndexerManager *itable_;
    // TermTable term_[10]; { "sex" -- GeneralIndexer, "age" -- SectionIndexer, "net" -- GeneralIndexer, "location" -- GeoHashIndexer }
};

