//
// cloriSearch core implementation
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
#include "internal/log.h"
#include "internal/json2pb.h"
#include "clorisearch.h"

class CloriSearch {
    bool Init(const std::string& schema_filename);
    bool Load(const std::string& filename);
    Search(const Query& query, int limit);

    inline InvertedIndex& inverted_index() const { return inverted_index_; }
    inline ForwardIndex& forward_index() const { return forward_index_; }
private:
    InvertedIndex inverted_index_;
    ForwardIndex forward_index_;
};

namespace cloris {

// TODO
CloriSearch::CloriSearch() {
}

// TODO
CloriSearch::~CloriSearch() {
}

bool CloriSearch::Init(const std::string& schema_source, SchemaFormat format, SourceType source_type) {
    if (src_type != DIRECT) {
        cLog(ERROR, "cloriSearch init failed: unsupport src_type");
        return false;
    }
    if (format != SM_JSON) {
        cLog(ERROR, "cloriSearch init failed: unsupport format-style schema now");
        return false;
    }
    IndexSchema schema;
    std::string err_msg;
    if (!json2pb::JsonToProtoMessage(schema_source, &schema, &err_msg)) {
        cLog(ERROR, "cloriSearch init failed: json2pb error");
        return false;
    }
    // init inverted index schema
    if (!inverted_index().Init(schema)) {
        cLog(ERROR, "cloriSearch init failed: inverted_index init failed");
        return false;
    }
    if (!forward_index().Init()) {
        cLog(ERROR, "cloriSearch init failed: forward_index init failed");
        return false;
    }
    cLog(INFO, "cloriSearch init success!");
    return true;
}

bool CloriSearch::Load(const std::string& source, int docid, SchemaFormat format, bool is_incremental) {
    if (format != SM_JSON) {
        cLog(ERROR, "unsupport format-style");
        return false;
    }
    DNF dnf;
    std::string err_msg;
    if (!json2pb::JsonToProtoMessage(source, &dnf, &err_msg)) {
        cLog(ERROR, "CloriSearch load failed");
        return false;
    }
    inverted_index().Add(dnf, docid);
}


std::vector<int> CloriSearch::Search(const Query& query, int limit) {
    return inverted_index().Search(const Query& query, limit);
}

} // namespace cloris
