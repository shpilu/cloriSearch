//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include <algorithm>
#include "posting_list.h"
#include "conjunction_scorer.h"

namespace cloris {

ConjunctionScorer::~ConjunctionScorer() {
    for (auto &p : plists_) {
        p.ReclaimDocList();
    }
}

void ConjunctionScorer::AddPostingList(std::list<DocidNode>* doc_list, const ReclaimHandler& handler) {
    PostingList pl(doc_list, handler);
    plists_.push_back(pl);
}

std::vector<int> ConjunctionScorer::GetMatchedDocid(size_t k) {
    std::vector<int> ret;
    if (k == 0) {
        k = 1;
    }
    if (plists_.size() < k) {
        return ret;
    }
    int next_id;
    while (plists_[k - 1].CurrentEntry() != PostingList::EOL) {
        std::sort(plists_.begin(), plists_.end());
        if (plists_[0].CurrentEntry() == plists_[k - 1].CurrentEntry()) {
            // TODO support NOT IN
            next_id = plists_[k - 1].CurrentEntry().docid + 1;
            //
            // e.g. city NOT IN {'beijing', 'shanghai'}
            //
            if (!plists_[0].CurrentEntry().is_belong_to) {
                // Do nothing, just skip to next_id in [K, plists_.size())
            } else {
                ret.push_back(plists_[k - 1].CurrentEntry().docid);
            }
            // skip same docid, e.g. docid=2,2,2,2,2
            for (size_t L = k; L < plists_.size(); ++L) {
                if (plists_[L].CurrentEntry().docid < next_id) {
                    plists_[L].SkipTo(next_id);
                } else {
                    break;
                }
            }
        } else {
            next_id = plists_[k - 1].CurrentEntry().docid;
        }
        for (size_t L = 0; L < k; ++L) {
            plists_[L].SkipTo(next_id);
        }
    } 
    return ret;
}

} // namespace cloris
