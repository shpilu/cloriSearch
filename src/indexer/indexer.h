//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_INDEXER_H_
#define CLORIS_INDEXER_H_

#include <list>
#include "inverted_index.pb.h"
#include "term.h"

namespace cloris {

class Indexer {
public:
    Indexer(const std::string& name) : name_(name) {}
    virtual ~Indexer() { }
    virtual bool ParseTermsFromConjValue(std::vector<Term>& terms, const ConjValue& value) = 0; 
    virtual bool Add(const ConjValue& value, bool is_belong_to, int docid) = 0;
    virtual const std::list<DocidNode>* GetPostingLists(const Term& term) = 0;
protected:
    std::string name_;
    ValueType type_;
private:

};

} // namespace cloris

#endif // CLORIS_INDEXER_H_

