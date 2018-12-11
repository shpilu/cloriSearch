//
// cloriSearch main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef  CLORIS_CLORISEARCH_H_
#define  CLORIS_CLORISEARCH_H_

#include "inverted_index.h"
#include "forward_index.h"

namespace cloris {

enum IndexSchemaFormat {
    ISF_PB   = 0,
    ISF_JSON = 1,
    ISF_XML  = 2,
};

enum SourceType {
    DIRECT = 0,
    LOCAL = 1,
    ZOOKEEPER = 2,
};

class CloriSearch {
public:
    CloriSearch();
    ~CloriSearch();

    bool Init(const std::string& source, IndexSchemaFormat format, SourceType source_type = DIRECT);
    bool Add(const std::string& source, IndexSchemaFormat format, bool is_incremental = false);
    std::vector<int> Search(const Query& query, int limit = -1);

    inline InvertedIndex* inverted_index() { return &iidx_; }
    inline ForwardIndex*  forward_index()  { return &fidx_; }
private:
    InvertedIndex iidx_;
    ForwardIndex fidx_;
};

} // namespace cloris

#endif // CLORIS_CLORISEARCH_H_
