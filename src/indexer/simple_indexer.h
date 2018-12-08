//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_SIMPLE_INDEXER_H_
#define CLORIS_SIMPLE_INDEXER_H_

namespace cloris {

// we treat docId AS string-type now
// switch this to template in future
struct ItemListHead {
    void Add(int docid);

    int len;
    std::list<int> list;
    // skip_list<int> list;
};

void ItemListHead::Add(int docid) {
    auto p = list.begin();
    for (; p != list.end(); ++p) {
        if (*p >= docid) {
            break;
        }
    }
    p.insert_front(docid);
}

template <typename T> 
class SimpleIndexer : public Indexer {
public:
    virtual Add(AttributeValue& value, int docid);

private:
    std::string key_type_;
    std::unordered_map<T, ItemListHead> lists_;
};


template <typename T>
SimpleIndexer::SimpleIndexer() {
    int i_val;
    std::string s_val;
    if (typeid(T) == typeid(i_val)) {
        key_type_ = IntType;
    } else if (typeid(T) == typeid(s_val)) {
        key_type_ = StringType;
    }
}

template <typename T>
inline T ConvertTo(const AttributeValue& val) {
    return t;
}

template<int>
inline int ConvertTo(const AttributeValue& val) {
    return val.ival;
}

template<std::string>
inline std::string ConvertTo(const AttributeValue& val) {
    return val.sval;
}

template <typename T>
bool SimpleIndexer::Add(AttributeValue& value, int docid) {
    T val = ConvertTo<T>(value);
    if (lists_.find(val ) != lists_.end()) {
        lists_[val].Add(docid);
    }
}

} // namespace cloris

#endif // CLORIS_SIMPLE_INDEXER_H_
