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
    SECTION  = 4,
};

class Term {
public:
    Term(const std::string&);
    Term(const std::string&, int32_t);
    Term(const std::string&, bool);
    Term(const std::string&, const std::string&);
    ~Term() { }

    // TODO use template
    Term& operator=(int32_t val);
    Term& operator=(bool val);
    Term& operator=(const std::string& val);
    bool operator==(const Term& t) const;
    // explicit Term& operator=(int32_t val[2]);

    ValueType type() const { return type_; }
    const std::string& name() const { return value_; }
    const std::string& value() const { return value_; }

private:
    Term() = delete;
    ValueType type_;
    // all data type is stored in value_
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
