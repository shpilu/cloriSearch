//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include "internal/log.h"
#include "simple_indexer.h"

namespace cloris {

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

bool SimpleIndexer::Add(const ConjValue& value, bool is_belong_to, int docid) {
    std::vector<Term> terms;
    ParseTermsFromConjValue(name_, terms, value, key_type_);
    for (auto& term : terms) {
        if (lists_.find(term) == lists_.end()) {
            lists_.insert(std::pair<Term, InvertedList>(term, InvertedList()));
        }
        cLog(DEBUG, "add simple_indexer item:[name=%s, value=%s, docid=%d]", term.name().c_str(), term.value().c_str(), docid);
        lists_[term].Add(is_belong_to, docid);
    }
    return true;
}

const std::list<DocidNode>* SimpleIndexer::GetPostingLists(const Term& term) {
    if (lists_.find(term) != lists_.end()) {
        return &(lists_[term].doc_list()); 
    } else {
        return NULL;
    }
}

} // namespace cloris
