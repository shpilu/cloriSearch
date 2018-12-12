//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include "inverted_list.h"

namespace cloris {

bool DocidNode::operator < (const DocidNode& dn) const {
    return ((this->docid < dn.docid) || 
            ((this->docid == dn.docid) && (!this->is_belong_to && dn.is_belong_to)));
}

bool DocidNode::operator == (const DocidNode& dn) const {
    return (this->docid == dn.docid) && (this->is_belong_to == dn.is_belong_to);
}

bool DocidNode::operator != (const DocidNode& dn) const {
    return (this->docid != dn.docid) || (this->is_belong_to != dn.is_belong_to);
}

void InvertedList::Add(bool is_belong_to, int docid) {
    auto iter = doc_list_.begin();
    for (; iter != doc_list_.end(); ++iter) {
        if (iter->docid >= docid) {
            break;
        }
    }
    doc_list_.insert(iter, DocidNode(docid, is_belong_to));
}

} // namespace cloris


