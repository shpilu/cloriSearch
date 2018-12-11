//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_SIMPLE_INDEXER_H_
#define CLORIS_SIMPLE_INDEXER_H_

#include <unordered_map>
#include <string>
#include "indexer.h"

namespace cloris {

// we treat docId AS string-type now
// switch this to template in future
struct ItemListHead {
    void Add(int docid);
    int len;
    std::list<int> doc_list;
    // skip_list<int> list;
};

class SimpleIndexer : public Indexer {
public:
    SimpleIndexer(const std::string& name, ValueType key_type);
    ~SimpleIndexer();
    virtual bool Add(const ConjValue& value, int docid);
    virtual std::list<int>* GetPostingLists(const Term& term);
private:
    SimpleIndexer() = delete;
    std::string name_;
    ValueType key_type_;
    std::unordered_map<Term, ItemListHead, TermHash> lists_;
};

} // namespace cloris

#endif // CLORIS_SIMPLE_INDEXER_H_
