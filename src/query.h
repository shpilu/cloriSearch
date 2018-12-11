//
// Query main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
// changeLog:

#ifndef CLORIS_SSMAP_H_
#define CLORIS_SSMAP_H_

// #include <set>
// #include <map>
// #include <vector>
// #include <queue>
// #include <memory>
// #include <functional>
// #include <boost/noncopyable.hpp>

#include <map>
#include "term.h"
#include "internal/generic_iterator.h"

namespace cloris {

class Query {
public:
    typedef typename GenericIterator<false, Term>::Iterator TermIterator;
    typedef typename GenericIterator<true, Term>::Iterator  ConstTermIterator;
    ConstTermIterator begin() const { return ConstTermIterator(terms_.begin()); } 
    ConstTermIterator end() const { return ConstTermIterator(terms_.end()); }
    TermIterator begin() { return TermIterator(terms_.begin()); }
    TermIterator end() { return TermIterator(terms_.end()); }

    Query() {}
    virtual ~Query() {}
    
    Term& operator[](const std::string& key);
    const Term& at(const std::string& key) const;

    size_t size() const { return terms_.size(); }
private:
    std::map<std::string, Term> terms_;
};

} // namepsace cloris

#endif // CLORIS_SSMAP_H_

