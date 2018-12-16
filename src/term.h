//
// inverted index main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORISEARCH_TERM_H_
#define CLORISEARCH_TERM_H_

#include <cstring>
#include <functional>
#include <string>

#define INTERVAL_TYPE_MASK  0x00010000
#define INTERVAL_FLAG_MASK  0x00000003
#define INTERVAL_LEFT_MASK  0x00000002
#define INTERVAL_RIGHT_MASK 0x00000001
#define BASIC_TYPE_MASK     0x0000000f

namespace cloris {

enum ValueType {
    NONE     = 0x00000000,
    BOOL     = 0x00000001,
    INT32    = 0x00000002,
    DOUBLE   = 0x00000004,
    STRING   = 0x00000008,
    MIN_INTERVAL_TYPE = 0x00010002,
    INT32_INTERVAL =  0x00010002,
    DOUBLE_INTERVAL = 0x00010004,
    STRING_INTERVAL = 0x00010008,
    MAX_INTERVAL_TYPE = 0x00010008,
};

class Term {
public:
    Term(const std::string&);
    Term(const std::string&, int32_t);
    Term(const std::string&, bool);
    Term(const std::string&, const std::string&);
    // Interval Expression and Encoding
    Term(const std::string&, int32_t, int32_t, int32_t);
    Term(const std::string&, double, double, int32_t);
    Term(const std::string&, const std::string&, const std::string&, int32_t);
    ~Term() { }

    Term& operator=(int32_t val);
    Term& operator=(bool val);
    Term& operator=(const char* val);
    Term& operator=(const std::string& val);
    bool operator==(const Term& t) const;

    ValueType type() const { return type_; }
    const std::string& name() const { return name_; }
    const std::string& value() const { return value_; }
    void* data() { return &value_[0]; }
    const void* data() const { return &value_[0]; }
    const void* left(size_t *llen = NULL) const ;
    const void* right(size_t *rlen = NULL) const;
    std::string print() const;

    // unsafe method, used only for XX_INTERVAL type
    int32_t flag() const { char p = value_[0]; return p; }

private:
    Term() = delete;
    ValueType type_;
    std::string name_;
    std::string value_;
};

struct TermHash {
    size_t operator()(const Term& t) const {
        return std::hash<std::string>()(t.value());
    }
};

} // namespace cloris

#endif // CLORISEARCH_TERM_H_
