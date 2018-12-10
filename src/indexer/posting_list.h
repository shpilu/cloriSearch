//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_POSTING_LIST_H_
#define CLORIS_POSTING_LIST_H_

namespace cloris {

class PostingList {
public:
    PostingList(std::list<int>* pl);
    ~PostingList() { }

    static EOL = -1,

    bool operator < (const PostingList& pl); 
    int CurrentEntry();
    void SkipTo(int docid);
private:
    std::list<int>* doc_list_;
    std::list<int>::iterator iter_;
};

} // namespace cloris

#endif // CLORIS_POSTING_LIST_H_
