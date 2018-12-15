//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include "internal/log.h"
#include "simple_indexer.h"

namespace cloris {

SimpleIndexer::SimpleIndexer(const std::string& name, ValueType type)
    : Indexer(name) {
    type_ = type;
}

SimpleIndexer::~SimpleIndexer() {
}

bool SimpleIndexer::ParseTermsFromConjValue(std::vector<Term>& terms, const ConjValue& value) {
    if (type_ == ValueType::INT32) {
        for (auto& p : value.ival()) {
            terms.push_back(Term(name_, p));
        }
    } else if (type_ == ValueType::STRING) {
        for (auto& p : value.sval()) {
            terms.push_back(Term(name_, p));
        }
    } else if (type_ == ValueType::BOOL) {
        if (value.has_bval()) {
            terms.push_back(Term(name_, value.bval()));
        }
    } else {
        // TODO
    }
    return true;
}

bool SimpleIndexer::Add(const ConjValue& value, bool is_belong_to, int docid) {
    std::vector<Term> terms;
    this->ParseTermsFromConjValue(terms, value);
    for (auto& term : terms) {
        if (inverted_lists_.find(term) == inverted_lists_.end()) {
            inverted_lists_.insert(std::pair<Term, InvertedList>(term, InvertedList()));
        }
        cLog(DEBUG, "add simple_indexer item:[name=%s, value=%s, docid=%d]", term.name().c_str(), term.value().c_str(), docid);
        inverted_lists_[term].Add(is_belong_to, docid);
    }
    return true;
}

const std::list<DocidNode>* SimpleIndexer::GetPostingLists(const Term& term) {
    if (inverted_lists_.find(term) != inverted_lists_.end()) {
        return &(inverted_lists_[term].doc_list()); 
    } else {
        return NULL;
    }
}

} // namespace cloris
