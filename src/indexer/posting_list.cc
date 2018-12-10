//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include "posting_list.h"

namespace cloris {

PostingList::PostingList(std::list<int>* pl) : doc_list_(pl) {
    iter_ = doc_list_->begin();
}

bool PostingList::operator < (const PostingList& pl) {
    if (pl.CurrentEntry() == EOL) {
        return true;
    }
    return (this->CurrentEntry() < pl.CurrentEntry());
}

int PostingList::CurrentEntry() {
    if (iter_ == doc_list->end()) {
        return EOL;
    } else {
        return *iter_;
    }
}

int PostingList::SkipTo(int docid) {
    while ((iter_ != doc_list_->end()) && (*iter < docid)) {
        ++iter_;
    }
}

void ConjunctionScorer::AddPostingList(std::list<int>* doc_list) {
    PostingList pl(doc_list_);
    plists_.push_back(pl);
}

} // namespace cloris
