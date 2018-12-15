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

//
// the encoding if int32 interval is
// | char | int32 | int32 |
// --------------------------
// | flag | left  | right |
//
Term::Term(const std::string& name, int32_t left, int32_t right, int32_t flag) {
    size_t len = sizeof(char) + sizeof(int32_t) * 2;
    char tmp[len];
    tmp[0] = flag & BASIC_TYPE_MASK;
    *(reinterpret_cast<int32_t*>(tmp[sizeof(char)])) = left;
    *(reinterpret_cast<int32_t*>(tmp[sizeof(char) + sizeof(int32_t)])) = right;
    value_.reserve(len);
    value_.assign(reinterpret_cast<char*>(&tmp), len);
    type_ = ValueType::INT32_INTERVAL;
};

//
// the encoding if double interval is
// | char | double | double |
// --------------------------
// | flag |  left  |  right |
//
Term::Term(const std::string& name, double left, double right, int32_t flag) {
    size_t len = sizeof(char) + sizeof(double) * 2;
    char tmp[len];
    tmp[0] = flag & BASIC_TYPE_MASK;
    *(reinterpret_cast<double*>(tmp[sizeof(char)])) = left;
    *(reinterpret_cast<double*>(tmp[sizeof(char) + sizeof(double)])) = right;
    value_.reserve(len);
    value_.assign(reinterpret_cast<char*>(&tmp), len);
    type_ = ValueType::DOUBLE_INTERVAL;
};

//
// the encoding if string interval is
// | char | size_t | string | size_t | string |
// --------------------------------------------
// | flag |  len1  |  left  |  len2  | right  |
//
Term::Term(const std::string& name, const std::string& left, const std::string& right, int32_t flag) {
    size_t len = sizeof(char) + sizeof(size_t) * 2 + left.length() + right.length();
    value_.reserve(len);
    value_[0] = flag & BASIC_TYPE_MASK;
    *(reinterpret_cast<size_t*>(value_[sizeof(char)])) = left.length();
    memcpy(&value_[sizeof(char) + sizeof(size_t)], left.data(), left.length());
    *(reinterpret_cast<size_t*>(value_[sizeof(char) + sizeof(size_t) + left.length()])) = right.length();
    memcpy(&value_[sizeof(char) + sizeof(size_t) * 2 + left.length()], right.data(), right.length());
    type_ = ValueType::STRING_INTERVAL;
};

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

const void* Term::left(size_t *llen) const {
    const void *p = NULL;
    if (type_ == ValueType::INT32_INTERVAL) {
        if (llen) {
            *llen = sizeof(int32_t);
        }
        p = &(value_[1]);
    } else if (type_ == ValueType::DOUBLE_INTERVAL) {
        if (llen) {
            *llen = sizeof(double);
        }
        p = &(value_[1]);
    } else if (type_ == ValueType::STRING_INTERVAL) {
        if (llen) {
            *llen = *reinterpret_cast<size_t*>(value_[1]);
        }
        p = &value_[1 + sizeof(size_t)];
    }
    return p; 
}

const void* Term::right(size_t *rlen) const {
    const void *p = NULL;
    if (type_ == ValueType::INT32_INTERVAL) {
        if (rlen) {
            *rlen = sizeof(int32_t);
        }
        p = &(value_[1 + sizeof(int32_t)]);
    } else if (type_ == ValueType::DOUBLE_INTERVAL) {
        if (rlen) {
            *rlen = sizeof(double);
        }
        p = &(value_[1 + sizeof(double)]);
    } else if (type_ == ValueType::STRING_INTERVAL) {
        if (rlen) {
            *rlen = *reinterpret_cast<size_t*>(value_[1 + sizeof(size_t) + (*reinterpret_cast<size_t*>(value_[1]))]);
        }
        p = &value_[1 + (sizeof(size_t) * 2) + (*reinterpret_cast<size_t*>(value_[1]))];
    }
    return p; 
}

} // namespace cloris
