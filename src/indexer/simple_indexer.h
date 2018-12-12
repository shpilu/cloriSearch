//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_SIMPLE_INDEXER_H_
#define CLORIS_SIMPLE_INDEXER_H_

#include <unordered_map>
#include <string>
#include "inverted_list.h"
#include "indexer.h"

namespace cloris {

class SimpleIndexer : public Indexer {
public:
    SimpleIndexer(const std::string& name, ValueType key_type);
    ~SimpleIndexer();
    virtual bool Add(const ConjValue& value, bool is_belong_to, int docid);
    virtual const std::list<DocidNode>* GetPostingLists(const Term& term);
private:
    SimpleIndexer() = delete;
    std::string name_;
    ValueType key_type_;
    std::unordered_map<Term, InvertedList, TermHash> lists_;
};

} // namespace cloris

#endif // CLORIS_SIMPLE_INDEXER_H_
