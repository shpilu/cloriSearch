//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_INVERTED_LIST_H_
#define CLORIS_INVERTED_LIST_H_

#include <unistd.h>
#include <list>

namespace cloris {

//
// we just treat docId AS int-type now
//
struct DocidNode {
    DocidNode(int _docid, bool _is_belong_to) : docid(_docid), is_belong_to(_is_belong_to) {}
    bool operator < (const DocidNode& dn) const; 
    bool operator == (const DocidNode& dn) const; 
    bool operator != (const DocidNode& dn) const; 

    int docid;
    bool is_belong_to;
};

// TODO switch general list to skip list 
class InvertedList {
public:
    InvertedList() {}
    ~InvertedList() {}
    void Add(bool is_belong_to, int docid); 
    void Copy(const InvertedList& other);
    const std::list<DocidNode>& doc_list() const { return doc_list_; }
    size_t length() const { return doc_list_.size(); }
private:
    std::list<DocidNode> doc_list_;
    // skip_list<DocidNode> list;
};

} // namespace cloris

#endif // CLORIS_INVERTED_LIST_H_
