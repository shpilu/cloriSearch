//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include "posting_list.h"

namespace cloris {

const DocidNode PostingList::EOL(DN_BAD_DOCID, false);

PostingList::PostingList(std::list<DocidNode>* pl, ReclaimHandler handler) 
    : doc_list_(pl), 
      handler_(handler) {
    iter_ = doc_list_->begin();
}

PostingList::~PostingList() { 
}

void PostingList::ReclaimDocList() {
    if (handler_) {
        handler_(doc_list_);
    }
}

bool PostingList::operator < (const PostingList& pl) const {
    if (pl.CurrentEntry() == EOL) {
        return true;
    }
    return (this->CurrentEntry() < pl.CurrentEntry());
}

const DocidNode& PostingList::CurrentEntry() const {
    if (iter_ == doc_list_->end()) {
        return EOL;
    } else {
        return *iter_;
    }
}

void PostingList::SkipTo(int docid) {
    while ((iter_ != doc_list_->end()) && (iter_->docid < docid)) {
        ++iter_;
    }
}

} // namespace cloris
