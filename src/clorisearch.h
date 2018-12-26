//
// cloriSearch main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef  CLORIS_CLORISEARCH_H_
#define  CLORIS_CLORISEARCH_H_

#ifdef ENABLE_PERSIST
    #include <leveldb/db.h>
#endif
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

struct CloriSearchOptions {
    std::string source;
    IndexSchemaFormat format;
    SourceType source_type;
    bool enable_persistence;
    std::string meta_dir;
    std::string inverted_list_dir;
};

class CloriSearch {
public:
    CloriSearch();
    ~CloriSearch();

    bool Init(const std::string& source, IndexSchemaFormat format, SourceType source_type = DIRECT);
    bool Init(const CloriSearchOptions& options);
    bool Add(const std::string& source, IndexSchemaFormat format, bool is_incremental = false);
    bool PersistToDatabase(const DNF& dnf);
    std::vector<int> Search(const Query& query, int limit = -1);

    inline InvertedIndex* inverted_index() { return &iidx_; }
    inline ForwardIndex*  forward_index()  { return &fidx_; }
    inline bool enable_persistence() const { return enable_persistence_; }
    inline const std::string& meta_dir() const     { return meta_dir_; }
    inline const std::string& inverted_list_dir() const { return inverted_list_dir_; }
private:
    InvertedIndex iidx_;
    ForwardIndex fidx_;
    bool enable_persistence_;
    std::string meta_dir_;
    std::string inverted_list_dir_;
#ifdef ENABLE_PERSIST    
    leveldb::DB *db_meta_;
    leveldb::DB *db_inverted_list_;
#endif
};

} // namespace cloris

#endif // CLORIS_CLORISEARCH_H_
