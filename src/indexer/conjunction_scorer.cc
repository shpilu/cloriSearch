//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include "conjunction_scorer.h"

namespace cloris {

class PostingList {
public:
    PostingList(std::list<int>* pl);
    ~PostingList() { }

    static EOL = -1,

    int CurrentEntry();
    void SkipTo(int docid);
private:
    std::list<int>* doc_list_;
    std::list<int>::iterator iter_;
};

PostingList::PostingList(std::list<int>* pl) : doc_list_(pl) {
    iter_ = doc_list_->begin();
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

std::vector<int> ConjunctionScorer::GetMatchedDocid(int k) {
    std::vector<int> ret;
    if (k == 0) {
        k = 1;
    }
    if (plists_.size() < k) {
        return ret;
    }
    int next_id;
    while (plists_[k - 1].CurrentEntry() != PostingList::EOL) {
        sort(plists_.begin(), plists_.end());
    } 
    if (plists_[0].CurrentEntry() == plists_[k - 1].CurrentEntry()) {
        // TODO support NOT IN
        next_id = plists_[k - 1].CurrentEntry() + 1;
        if (false) {
        } else {
            ret.push_back(plists_[k - 1].CurrentEntry());
            // K=2, 2,2,2,2
            for (int L = k; L < plists_.size(); ++L) {
                if (plists_[L] < next_id) {
                    plists_[L].SkipTo(next_id);
                } else {
                    break;
                }
            }
        }
    } else {
        next_id = plists_[k - 1].CurrentEntry();
    }
    for (int L = 0; L < k; ++L) {
        plists_[L].SkipTo(next_id);
    }
}

} // namespace cloris
