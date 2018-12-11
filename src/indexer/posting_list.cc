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

bool PostingList::operator < (const PostingList& pl) const {
    if (pl.CurrentEntry() == EOL) {
        return true;
    }
    return (this->CurrentEntry() < pl.CurrentEntry());
}

int PostingList::CurrentEntry() const {
    if (iter_ == doc_list_->end()) {
        return EOL;
    } else {
        return *iter_;
    }
}

void PostingList::SkipTo(int docid) {
    while ((iter_ != doc_list_->end()) && (*iter_ < docid)) {
        ++iter_;
    }
}

} // namespace cloris
