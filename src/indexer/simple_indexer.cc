//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include "simple_indexer.h"

namespace cloris {

void ItemListHead::Add(int docid) {
    auto iter = doc_list.begin();
    for (; iter != doc_list.end(); ++iter) {
        if (*iter >= docid) {
            break;
        }
    }
    doc_list.insert(iter, docid);
}

SimpleIndexer::SimpleIndexer(const std::string& name, ValueType key_type)
    : name_(name),
      key_type_(key_type) {
}

SimpleIndexer::~SimpleIndexer() {
}

static bool ParseTermsFromConjValue(const std::string& name, std::vector<Term>& terms, const ConjValue& value, ValueType type) {
    if (type == ValueType::INT32) {
        for (auto& p : value.ival()) {
            terms.push_back(Term(name, p));
        }
    } else if (type == ValueType::STRING) {
        for (auto& p : value.sval()) {
            terms.push_back(Term(name, p));
        }
    } else if (type == ValueType::BOOL) {
        if (value.has_bval()) {
            terms.push_back(Term(name, value.bval()));
        }
    } else {
        // TODO
    }
    return true;
}

bool SimpleIndexer::Add(const ConjValue& value, int docid) {
    std::vector<Term> terms;
    ParseTermsFromConjValue(name_, terms, value, key_type_);
    for (auto& term : terms) {
        if (lists_.find(term) == lists_.end()) {
            lists_.insert(std::pair<Term, ItemListHead>(term, ItemListHead()));
        }
        lists_[term].Add(docid);
    }
    return true;
}

std::list<int>* SimpleIndexer::GetPostingLists(const Term& term) {
    if (lists_.find(term) != lists_.end()) {
        return &(lists_[term].doc_list);
    } else {
        return NULL;
    }
}

} // namespace cloris
