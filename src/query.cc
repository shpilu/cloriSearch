//
// cloriSearch core implementation
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
// #include <string.h>
// #include "json/json.h"
// #include "utils/olog.h"
// #include "utils/str_util.h"

#include "query.h"

namespace cloris {

CrowdMatchFunc CondMeta::match_crowd = NULL;

CondMeta::CondMeta() : type_(tNone) {
    value_.section_[0] = 0;
    value_.section_[1] = 0;
}

CondMeta::~CondMeta() {
}

CondMeta& CondMeta::operator=(int32_t val) {
    value_.int32_ = val;
    type_ = tInt32;
    return *this;
}

CondMeta& CondMeta::operator=(bool val) {
    value_.bool_ = val;
    type_ = tBool;
    return *this;
}

CondMeta& CondMeta::resolveReference(const char* val) {
    str_value_ = val;
    type_ = tString;
    return *this;
}

CondMeta& CondMeta::operator=(const char* val) {
    return resolveReference(val);
}

CondMeta& CondMeta::operator=(const std::string& val) {
    return resolveReference(val.c_str());
}

CondMeta& CondMeta::operator=(int val[2]) {
    value_.section_[0] = val[0];
    value_.section_[1] = val[1];
    type_ = tSection;
    return *this;
}

CondMeta& CondMeta::operator=(ValueType type) {
    type_ = type;
    return *this;
}

bool CondMeta::operator==(const CondMeta& b) const {
    if (type_ != b.type()) {
        return false;
    }
    // TODO
    // 有缺陷
    if (type_ != tString) {
        return (value_.section_[0] == b.value().section_[0]) && (value_.section_[1] == b.value().section_[1]);
    } else {
        return str_value_ == b.str_value(); 
    }
}
bool CondMeta::set_val(const std::string& str, ValueType type) {
    if ((type == tAntiIncluded) || (type == tAntiExcluded)) {
        str_value_ = str;
        return true;
    } else {
        return false;
    }
}

void CondMeta::push_string(const std::string& str, ValueType type) {
    str_vec_.push_back(str);
    type_ = type;
}

CondMeta& ssmap::operator[](const std::string& key) {
    if (map_.find(key) != map_.end()) {
        return map_[key];
    } else {
        CondMeta meta;
        map_.insert(std::pair<std::string, CondMeta>(key, meta));
        CondMeta& val = map_[key]; 
        return val;
    }
}

// unsafe
const CondMeta& ssmap::at(const std::string& key) const {
    return map_.at(key);
}

CondMeta& ssmap::operator[](const char* key) {
    if (map_.find(key) != map_.end()) {
        return map_[key];
    } else {
        CondMeta meta;
        map_.insert(std::pair<std::string, CondMeta>(key, meta));
        CondMeta& val = map_[key]; 
        return val;
    }
}

std::string CondMeta::toString() const {
    if (type_ == tBool) {
        return value_.bool_  ? "true" : "false";
    } else if (type_ == tInt32) {
        return std::to_string(value_.int32_);
    } else if (type_ == tSection) {
        return ("[" + std::to_string(value_.section_[0]) + "," + std::to_string(value_.section_[1]) + "]"); 
    } else if (type_ == tString) {
        return str_value_;
    } else if (type_ == tStringArray) {
        return "unrealized";
    } else {
        return "unrealized";
    }
}

bool CondMeta::Exists(const std::string& key) const {
    for (auto &p : str_vec_) {
        if (key == p) {
            return true;
        }
    }
    return false;
}

bool CondMeta::Match(const CondMeta& val) const {
    bool match = false;
    if (type_ == tNone) {
        //match = false;
    } else if (type_ < tSection) {
        if (type_ != val.type()) {
            return false;
        }
        if (type_ == tBool) {
            match = (this->value_.bool_ == val.value().bool_);
        } else if (type_ == tInt32) {
            match = (this->value_.int32_ == val.value().int32_);
        } else if (type_ == tString) {
            match = (str_value_ == val.str_value());
        } else {
        }
    } else if (type_ == tSection) {
        if (val.type() != tInt32) {
            // match = false;
        } else {
            int32_t begin = this->value_.section_[0];
            int32_t end   = this->value_.section_[1];
            if ((val.value().int32_ >= begin) && (val.value().int32_ <= end)) {
                match = true;
            }
        }
    } else if ((type_ == tInCrowd || type_ == tExCrowd)) {
        if ((val.type() == tString) && CondMeta::match_crowd && CondMeta::match_crowd(str_vec_, val.str_value(), type_)) {
            match = true;
        }
    } else if ((type_ == tAntiIncluded|| type_ == tAntiExcluded)) {
        if ((val.type() == tStringArray) && val.Exists(str_value_)) {
            match = (type_ == tAntiIncluded); 
        } else {
            match = (type_ == tAntiExcluded);
        }
    } else {
        // TODO
    }
    return match;
}

ssmap::ssmap() 
    : priority_(0),
      ecpm_(0.0),
      group_id_("none"),
      id_("0"),
      value_("") {
}

ssmap::ssmap(const std::string& group_id) 
    : priority_(0),
      ecpm_(0.0),
      group_id_(group_id),
      id_("0"),
      value_("") {
}

ssmap::ssmap(const std::string& group_id, const std::string& id) 
    : priority_(0),
      ecpm_(0.0),
      group_id_(group_id),
      id_(id),
      value_("") {
}

bool ssmap::hasKey(const std::string& key) const {
    return (map_.find(key) != map_.end());
}

bool ssmap::AddMeta(const std::string& key, const std::string& params, ValueType type) {
    bool ok(false);
    CondMeta meta;
    if (type == tString) {
        meta = params;
        ok = true;
    }
    if (ok) {
        map_.insert(std::pair<std::string, CondMeta>(key, meta));
    }
    return ok;
}

bool ssmap::Parse(const std::string& cond, const std::string& rule, std::string& err_msg) {
    Json::Reader reader;
    Json::Value jcond;
    if ((!reader.parse(cond, jcond))) {
        err_msg = "bad cond, not json format, cond=" + cond + ", rule=" + rule;
        return false;
    } 
    if (!jcond.isObject()) {
        err_msg = "bad cond, not json object, cond=" + cond + ", rule=" + rule;
        return false;
    }
    if (jcond["group_id"].isString() && jcond["id"].isString() && jcond["priority"].isInt() && jcond["rule"].isObject()) {
        for (auto iter = jcond["rule"].begin(); iter != jcond["rule"].end(); ++iter) {
            CondMeta meta;
            std::string key = iter.name();
            LOG(DEBUG, "RULE_PARSE, key=" + key);
            Json::Value vi = jcond["rule"][key];
            if (vi.isBool()) {
                meta = vi.asBool();
            } else if (vi.isInt()) {
                meta = vi.asInt();
            } else if (vi.isString()) {
                meta = vi.asString();
            } else if (vi.isArray() && (vi.size() == 2) && vi[0].isInt()) {
                int val[2];
                val[0] = vi[0].asInt();
                val[1] = vi[1].asInt();
                meta = val;
            } else if (vi.isObject() && vi["type"].isString()){
                std::string type = vi["type"].asString();
                if ((type == "in_crowd") && (vi["value"].isArray() && (vi["value"].size() > 0) && vi["value"][0].isString())) {
                    for (auto &p : vi["value"]) {
                        meta.push_string(p.asString(), tInCrowd);
                    }
                } else if ((type == "ex_crowd") && (vi["value"].isArray() && (vi["value"].size() > 0) && vi["value"][0].isString())) {
                    for (auto &p : vi["value"]) {
                        meta.push_string(p.asString(), tExCrowd);
                    }
                } else if ((type == "anti-exclued") && vi["value"].isString()) {
                    // support only one...
                    std::string str(vi["value"].asString());
                    if (str.size() > 0) {
                        meta.set_val(str, tAntiExcluded);
                    }
                } else {
                    LOG(ERROR, "unrecognized valuetype");
                    continue;
                }
            } else {
                LOG(ERROR, "unrecognized valuetype");
                continue;
            }
            map_.insert(std::pair<std::string, CondMeta>(key, meta));
        }
        // regard as default rule
        if (map_.size() == 0) {
            //do noting
        }
        priority_ = jcond["priority"].asInt();
        group_id_ = jcond["group_id"].asString();
        id_       = jcond["id"].asString();
        if (jcond["ecpm"].isDouble()) {
            ecpm_ = jcond["ecpm"].asDouble();
        }
        value_    = rule; 
        return true;
    } else {
        err_msg = "bad ssmap, check group_id/id/priority/rule";
        return false;
    }
}

std::string ssmap::dumpRule() {
    std::string ret("group_id=" + group_id_);
    for (auto &p : map_) {
        util::kv_pair_append(ret, p.first.c_str(), p.second.toString());
    }
    return ret;
}

bool ssmap::EqualTo(const ssmap& input) {
    // std::map<std::string, CondMeta> map_;
    if (rule_num() != input.rule_num()) {
        return false;
    }
    for (auto &p : map_) {
        if (!input.hasMeta(p.first, p.second)) {
            return false;
        }
    }
    return true;
    
}

bool ssmap::hasMeta(const std::string& key, const CondMeta& meta) const {
    return (map_.find(key) != map_.end()) && (map_.at(key) == meta);
}

bool ssmap::Match(const ssmap& input) {
    for (auto &p : map_) {
        std::string key(p.first);
        if (!input.hasKey(key)) {
            LOG(DEBUG, "key not found, key=" + key + ", id=" + this->id());
            return false;
        }
        CondMeta& meta1 = p.second;
        const CondMeta& meta2 = input.at(key);
        if (!meta1.Match(meta2)) {
            return false;
        }
    }
    return true;
}

bool ssmap::Match(const ssmap& input, std::string& err_msg) {
    for (auto &p : map_) {
        std::string key(p.first);
        if (!input.hasKey(key)) {
            err_msg = "no_key=" + key;
            return false;
        }
        const CondMeta& meta1 = p.second;
        const CondMeta& meta2 = input.at(key);
        if (!meta1.Match(meta2)) {
            err_msg = "input_meta=" + meta2.toString() + ", source=" + meta1.toString();
            return false;
        }
    }
    return true;
}

} // namepace cloris
