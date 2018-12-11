//
// inverted index main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include "term.h"

namespace cloris {

Term::Term(const std::string& name) 
    : type_(ValueType::NONE), 
      name_(name),
      value_("") {
}

Term::Term(const std::string& name, int32_t val) {
    name_ = name;
    value_.assign(reinterpret_cast<char*>(&val), sizeof(int32_t));
    type_ = ValueType::INT32;
}

Term::Term(const std::string& name, bool val) {
    name_ = name;
    value_.assign(reinterpret_cast<char*>(&val), sizeof(bool));
    type_ = ValueType::BOOL;
}

Term::Term(const std::string& name, const std::string& val) {
    name_ = name;
    value_.assign(val);
    type_ = ValueType::STRING;
}

Term& Term::operator=(int32_t val) {
    if (value_.size() > sizeof(int32_t)) {
        value_.resize(sizeof(int32_t));
    }
    value_.assign(reinterpret_cast<char*>(&val), sizeof(int32_t));
    type_ = ValueType::INT32;
    return *this;
}

Term& Term::operator=(bool val) {
    if (value_.size() > sizeof(bool)) {
        value_.resize(sizeof(bool));
    }
    value_.assign(reinterpret_cast<char*>(&val), sizeof(bool));
    type_ = ValueType::BOOL;
    return *this;
}

// NOTE:
Term& Term::operator=(const char* val) {
    value_.assign(val);
    type_ = ValueType::STRING;
    return *this;
}

Term& Term::operator=(const std::string& val) {
    value_.assign(val);
    type_ = ValueType::STRING;
    return *this;
}

bool Term::operator==(const Term& t) const {
    return ((type_ == t.type()) && (value_ == t.value())); 
}

} // namespace cloris
