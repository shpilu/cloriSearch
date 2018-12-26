//
// cloriSearch core implementation
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
#include "internal/log.h"
#include "json2pb/json2pb.h"
#include "clorisearch.h"

namespace cloris {

CloriSearch::CloriSearch()
    : enable_persistence_(false) 
#ifdef ENABLE_PERSIST
      , db_meta_(NULL),
      db_inverted_list_(NULL) 
#endif
{

}

CloriSearch::~CloriSearch() {
#ifdef ENABLE_PERSIST
    if (db_meta_) {
        delete db_meta_;
    }
    if (db_inverted_list_) {
        delete db_inverted_list_;
    }
#endif
}

bool CloriSearch::Init(const std::string& source, IndexSchemaFormat format, SourceType source_type) {
    if (source_type != DIRECT) {
        cLog(ERROR, "cloriSearch init failed: unsupport source_type");
        return false;
    }
    if (format != ISF_JSON) {
        cLog(ERROR, "cloriSearch init failed: unsupport format-style schema now");
        return false;
    }
    IndexSchema schema;
    std::string err_msg;
    if (!json2pb::JsonToProtoMessage(source, &schema, &err_msg)) {
        cLog(ERROR, "cloriSearch init failed: json2pb error=%s", err_msg.c_str());
        return false;
    }
    // init inverted index schema
    if (!inverted_index()->Init(schema, err_msg)) {
        cLog(ERROR, "cloriSearch init failed: inverted_index init failed");
        return false;
    }
    if (!forward_index()->Init()) {
        cLog(ERROR, "cloriSearch init failed: forward_index init failed");
        return false;
    }
    cLog(INFO, "cloriSearch init success!");
    return true;
}

bool CloriSearch::Init(const CloriSearchOptions& options) {
    bool ok = this->Init(options.source, options.format, options.source_type);
    if (ok) {
        this->enable_persistence_ = options.enable_persistence;
        this->meta_dir_ = options.meta_dir;
        this->inverted_list_dir_ = options.inverted_list_dir;
#ifdef ENABLE_PERSIT
        if (this->enable_persistence_) {
            leveldb::Options options;
            options.create_if_missing = true;
            leveldb::Status status = leveldb::DB::Open(options, this->meta_dir_, &this->db_meta_);
            if (!status.ok()) {
                return false;
            }
            status = leveldb::DB::Open(options, this->inverted_list_dir_, &this->db_inverted_list_);
            return status.ok();
        }
#endif
    }
    return ok;
}

bool CloriSearch::PersistToDatabase(const DNF& dnf) {
    bool ok = true;
#ifdef ENABLE_PERSIT    
    std::string proto_str;
    if (dnf.SerializeToString(&proto_str)) {
        status = this->db_inverted_list_->Put(leveldb::WriteOptions(), std::to_string(dnf.docid()), proto_str);
        ok = status.ok();
    } else {
        cLog(ERROR, "[PERSISTENCE]dnf serialization failed");
        ok = false;
    }
    delete db;
    cLog(INFO, "[PERSISTENCE] success, docid=%d", dnf.docid());
#endif
    return ok; 
}

bool CloriSearch::Add(const std::string& source, IndexSchemaFormat format, bool is_incremental) {
    if (format != ISF_JSON) {
        cLog(ERROR, "unsupport format-style");
        return false;
    }
    DNF dnf;
    std::string err_msg;
    if (!json2pb::JsonToProtoMessage(source, &dnf, &err_msg)) {
        cLog(ERROR, "CloriSearch load failed:%s", err_msg.c_str());
        return false;
    }
    inverted_index()->Add(dnf, is_incremental);
    // Data persistence
    if (this->enable_persistence()) {
        this->PersistToDatabase(dnf);
    }
    return true;
}

std::vector<int> CloriSearch::Search(const Query& query, int limit) {
    return inverted_index()->Search(query, limit);
}

} // namespace cloris
