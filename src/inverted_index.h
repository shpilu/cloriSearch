//
// inverted index main class definition
// InvertedIndex is the manager class of inverted index
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_INVERTED_INDEX_H_
#define CLORIS_INVERTED_INDEX_H_

#include <set>
#include "index_schema.pb.h"
#include "inverted_index.pb.h"
#include "query.h"

namespace cloris {

class IndexerManager;

class InvertedIndex {
public:
    InvertedIndex();
    ~InvertedIndex();

    bool Init(const IndexSchema& schema, std::string& err_msg);
    bool Add(const DNF& dnf, bool is_incremental);
    bool Add(const Disjunction& disjunction, int docid, bool is_incremental);
    bool Update(DNF *dnf, int docid);
    bool Del(int docid);
    std::vector<int> Search(const Query& query, int limit);
    void GetStandardQuery(const Query& query, Query& std_query);
private:
    std::set<std::string> terms_; // age, sex, city...
    int max_conj_;
    // 10 mean the max -- is 10
    IndexerManager *itable_;
};

} // namespace cloris

#endif // CLORIS_INVERTED_INDEX_H_ 
