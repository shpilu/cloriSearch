//
// cloriSearch main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_CLORISEARCH_H_
#define  CLORIS_CLORISEARCH_H_

namespace cloris {

enum SchemaFormat {
    SM_PB   = 0,
    SM_JSON = 1,
    SM_XML  = 2,
};

enum SourceType {
    DIRECT = 0;
    LOCAL = 1;
    ZOOKEEPER = 2;
};

//
class InvertedIndex {
public:
    bool Init(IndexSchema* schema);
    Build();
    bool Add(DNF *dnf, int docid, bool is_incremental);
    bool Update(DNF *dnf, int docid);
    bool Del(int docid);
    std::vector<int> Search(const Query& query, int limit);
private:
    vector<std::string> schemas_; // age, sex, city...
    // 10 mean the max -- is 10
    TermTable* term_;
    // TermTable term_[10]; { "sex" -- GeneralIndexer, "age" -- SectionIndexer, "net" -- GeneralIndexer, "location" -- GeoHashIndexer }
};


class BaseIndexer {
    virtual void Init();
};

typedef std::map<std::string, Indexer*> TermTable;

// 
// "age" => 10-20 --> 
// "sex" => m0, m1, m2, m3 ... each mx ~ "man" ---, "woman"---
//

class ForwardIndex {

private:
    int pos[]
    // *p...
};

class CloriSearch {
public:
    ClorisSearch();
    ~ClorisSearch();

    bool Init(const std::string& schema_source, SchemaFormat format, SourceType source_type = DIRECT);
    // load...
    bool Load(const std::string& source, int docid, SchemaFormat format, bool is_incremental = false);
    int SearchTop(cosnt Query& query, ss);
    std::vector<int> Search(const Query& query, int limit = -1);
private:
    InvertedIndex iidx_;
    ForwardIndex fidx_;
};

} // namespace cloris




#endif // CLORIS_CLORISEARCH_H_
