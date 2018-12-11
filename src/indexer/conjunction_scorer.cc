//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include <algorithm>
#include "posting_list.h"
#include "conjunction_scorer.h"

namespace cloris {

void ConjunctionScorer::AddPostingList(std::list<int>* doc_list) {
    PostingList pl(doc_list);
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
            next_id = plists_[k - 1].CurrentEntry() + 1;
            if (false) {
            } else {
                ret.push_back(plists_[k - 1].CurrentEntry());
                // K=2,2,2,2,2
                for (size_t L = k; L < plists_.size(); ++L) {
                    if (plists_[L].CurrentEntry() < next_id) {
                        plists_[L].SkipTo(next_id);
                    } else {
                        break;
                    }
                }
            }
        } else {
            next_id = plists_[k - 1].CurrentEntry();
        }
        for (size_t L = 0; L < k; ++L) {
            plists_[L].SkipTo(next_id);
        }
    } 
    return ret;
}

} // namespace cloris
