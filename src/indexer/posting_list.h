//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_POSTING_LIST_H_
#define CLORIS_POSTING_LIST_H_

//
// you can not use this value as docid now
//
#define DN_BAD_DOCID -31415926 

#include <list>
#include <functional>
#include "inverted_list.h"

namespace cloris {

typedef std::function<void(std::list<DocidNode>*)> ReclaimHandler;

class PostingList {
public:
    const static DocidNode EOL;
    PostingList(std::list<DocidNode>* pl, ReclaimHandler handler);
    ~PostingList(); 
    bool operator < (const PostingList& pl) const ; 
    const DocidNode& CurrentEntry() const;
    void SkipTo(int docid);
    void ReclaimDocList();
private:
    std::list<DocidNode>* doc_list_;
    ReclaimHandler handler_;
    std::list<DocidNode>::iterator iter_;
};

} // namespace cloris

#endif // CLORIS_POSTING_LIST_H_

