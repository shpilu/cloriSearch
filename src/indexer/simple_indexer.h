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
    std::list<int> doc_list;
    // skip_list<int> list;
};

void ItemListHead::Add(int docid) {
    auto iter = doc_list.begin();
    for (; iter != doc_list.end(); ++iter) {
        if (*iter >= docid) {
            break;
        }
    }
    doc_list.insert(iter, docid);
}

template <typename T> 
class SimpleIndexer : public Indexer {
public:
    SimpleIndexer(const std::string& name, const std::string& key_type, const std::string& index_type);
    virtual bool Add(const TermValue& value, int docid);
    virtual std::list<int>* GetPostingLists(const TermValue& val);
private:
    std::string name_;
    std::string key_type_;
    std::string index_type_;
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
bool SimpleIndexer::Add(const TermValue& value, int docid) {
    T val = ConvertTo<T>(value);
    if (lists_.find(val ) == lists_.end()) {
        lists_.insert(std::pair<T, ItemListHead>(val, ItemListHead()));
    }
    lists_[val].Add(docid);
}

template <typename T>
std::list<int>* SimpleIndexer::GetPostingLists(const TermValue& val) {
    T val = ConvertTo<t>(value);
    if (lists_.find(val) != lists_.end()) {
        return &(lists_[val].doc_list);
    } else {
        return NULL;
    }
}

} // namespace cloris

#endif // CLORIS_SIMPLE_INDEXER_H_
