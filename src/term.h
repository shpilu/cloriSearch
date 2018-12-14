//
// inverted index main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORISEARCH_TERM_H_
#define CLORISEARCH_TERM_H_

#include <functional>
#include <string>


namespace cloris {

enum ValueType {
    NONE     = 0,
    BOOL     = 1,
    INT32    = 2,
    STRING   = 3,
    INT32_INTERVAL = 4,
    DOUBLE_INTERVAL = 5,
    STRING_INTERVAL = 6,
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
    void* left(size_t *llen = NULL);
    void* right(size_t *rlen = NULL);

    // unsafe method, used only for XX_INTERVAL type
    int32_t flag() { char p = value_[0]; return p; }

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
