//
// Query main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
// changeLog:

#ifndef CLORIS_SSMAP_H_
#define CLORIS_SSMAP_H_

// #include <set>
// #include <map>
// #include <vector>
// #include <queue>
// #include <memory>
// #include <functional>
// #include <boost/noncopyable.hpp>

namespace cloris {

enum ValueType {
    tNone    = 0,
    tBool    = 1,
    tInt32   = 2,
    tString  = 3,
    tSection = 4,
    tInCrowd = 5,
    tExCrowd = 6,
    tStringArray = 7,
    tAntiIncluded = 8,
    tAntiExcluded = 9,
};

union ValueHolder {
    int32_t int32_;
    double  real_;
    bool    bool_;
    int     section_[2];
};

typedef std::function<bool(const std::vector<std::string>&, const std::string&, ValueType)> CrowdMatchFunc;

class CondMeta {
public:
    static CrowdMatchFunc match_crowd;
public:
    CondMeta();
    ~CondMeta();

    bool Match(const CondMeta& val) const;
    CondMeta& resolveReference(const char* val);
    CondMeta& operator=(int32_t val);
    CondMeta& operator=(bool val);
    CondMeta& operator=(const char* val);
    CondMeta& operator=(const std::string& val);
    CondMeta& operator=(int val[2]);
    CondMeta& operator=(ValueType);
    bool operator==(const CondMeta& b) const;

    ValueHolder value() const { return value_; }
    const std::string& str_value() const { return str_value_; }
    ValueType type() const { return type_; }

    std::string toString() const;
    void push_string(const std::string& str, ValueType type);
    bool set_val(const std::string& str, ValueType type);
    bool Exists(const std::string& key) const;
private:
    ValueHolder value_;
    ValueType type_;
    std::string str_value_;
    std::vector<std::string> str_vec_;
};

class ssmap {
public:
    ssmap(); 
    explicit ssmap(const std::string& group_id);
    ssmap(const std::string& group_id, const std::string& id);
    ~ssmap() {}
    
    CondMeta& operator[](const char* key);
    CondMeta& operator[](const std::string& key);
    const CondMeta& at(const std::string& key) const;

    const std::string group_id() const { return group_id_; }
    const std::string value() const { return value_; }
    const std::string id() const { return id_; }
    int32_t priority() const { return priority_; }
    double ecpm() const { return ecpm_; }
    size_t rule_num() const { return map_.size(); }

    bool hasMeta(const std::string& key, const CondMeta& meta) const;

    void set_priority(int32_t priority) { priority_ = priority; }
    void set_value(const std::string& value) { value_ = value; }

    bool AddMeta(const std::string& key, const std::string& params, ValueType type);
    bool Parse(const std::string& cond, const std::string& rule, std::string& err_msg);
    bool hasKey(const std::string& key) const;
    bool Match(const ssmap& input);
    bool Match(const ssmap& input, std::string& err_msg); 
    bool EqualTo(const ssmap& input);
    std::string dumpRule();
private:
    std::map<std::string, CondMeta> map_;
    int32_t priority_;
    double ecpm_;
    std::string group_id_;
    std::string id_;
    std::string value_;
};

struct ssmap_cmp {
    bool operator ()(ssmap& a, ssmap& b) {
        if (a.priority() != b.priority()) {
            return a.priority() < b.priority();
        } else {
            return a.ecpm() < b.ecpm();
        }
    }
    bool operator ()(std::shared_ptr<ssmap> a, std::shared_ptr<ssmap> b) {
        if (a->priority() != b->priority()) {
            return a->priority() < b->priority();
        } else {
            return a->ecpm() < b->ecpm();
        }
    }
};

} // namepsace cloris

#endif // CLORIS_SSMAP_H_

