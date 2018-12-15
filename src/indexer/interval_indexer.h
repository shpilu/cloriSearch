//
// iterval indexer main class definition
// A generic interval indexer implementation, e.g. age ∈ [18, 25]
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
// TODO @James Wei
// 1.  It's bad efficient to 目前跳跃表的插入效率不高
//
// 2. 插入可能导致区间合并: e.g. Interval(10, 18] -- Docid(1, 2, 3, 4), 
// Interval(18, 22] -- Docid(1, 2, 3) 在插入Interval(10, 22] -- Docid(4) 
// 时应该合并区间为(10, 22] -- Docid(1, 2, 3, 4), 但目前没有合并区间
//
// some tips-- 
// 10-20 25-30
// we use: skip list
// 10 -15 15-20 20-28 28-30

#ifndef CLORIS_INTERVAL_INDEXER_H_
#define CLORIS_INTERVAL_INDEXER_H_

#include <stdexcept>  // std::logic_error
#include <type_traits> // std::is_same
#include <functional> // std::less
#include "third_party/cloriskip/skip_list.h"
#include "interval.h"
#include "indexer.h"

namespace cloris {

template <typename T, typename Comp = std::less<T>>
class IntervalNode : public Interval<T, Comp> {
public:
    operator bool() const { return this->is_empty() ? false: true; }
    bool operator < (const IntervalNode<T>& other) const { 
        if (this->is_empty() != other.is_empty()) {
            return this->is_empty();
        } else {
            if (this->is_empty()) {
                return false;
            } else {
                return this->right() < other.left();
            }
        }
    }
    IntervalNode(const Term& term, ValueType type); 
    IntervalNode(const Interval<T>& interval, const InvertedList& vlist);
    IntervalNode(const Interval<T>& interval);
    void Add(bool is_belong_to, int docid) { return list_.Add(is_belong_to, docid); }
    const InvertedList& list() const { return list_; }
private:
    InvertedList list_;
};

template<typename T, typename Comp>
IntervalNode<T, Comp>::IntervalNode(const Interval<T>& interval, const InvertedList& vlist) {
    this->Reset(interval);
    list_.Copy(vlist);
}

template<typename T, typename Comp>
IntervalNode<T, Comp>::IntervalNode(const Interval<T>& interval) {
    this->Reset(interval);
}

//
// template specialization for support data type
//
// string
template<typename T>
T convert(const void *p, size_t sz) {
    return T(static_cast<const char*>(p), sz);
}

template<>
int32_t convert(const void *p, size_t sz) {
    return *reinterpret_cast<const int32_t*>(p);
}

template<>
double convert(const void *p, size_t sz) {
    return *reinterpret_cast<const double*>(p);
}

// string
template<typename T>
T convert(const void *p) {
    return T(static_cast<const char*>(p));
}

template<>
int32_t convert(const void *p) {
    return *reinterpret_cast<const int32_t*>(p);
}

template<>
double convert(const void *p) {
    return *reinterpret_cast<const double*>(p);
}

// special for string type
template<typename T, typename Comp>
IntervalNode<T, Comp>::IntervalNode(const Term& term, ValueType type) {
    if (!(type & term.type() & BASIC_TYPE_MASK)) {
        this->set_empty();
        return;
    }
    // if term is a interval type
    if ((type & term.type()) & INTERVAL_TYPE_MASK) {
        size_t left_size;
        size_t right_size;
        const void *lptr = term.left(&left_size);
        const void *rptr = term.right(&right_size);
        T left = convert<T>(lptr, left_size);
        T right = convert<T>(rptr, right_size);
        this->Reset(left, right, term.flag());
    } else {
        // if term is not a interval type, extend it to [X, X]
        const void *ptr = term.data();
        this->Reset(convert<T>(ptr), convert<T>(ptr), INTERVAL_CLOSE_FLAG);
    }
}

template<typename T, typename Compare = std::less<T>>
class IntervalIndexer : public Indexer {
public:
    IntervalIndexer(const std::string& name);
    ~IntervalIndexer();
    bool ParseTermsFromConjValue(std::vector<Term>& terms, const ConjValue& value);
    bool Add(const Term& term, bool is_belong_to, int docid);
    virtual bool Add(const ConjValue& value, bool is_belong_to, int docid);
    virtual const std::list<DocidNode>* GetPostingLists(const Term& term);
private:
    goodliffe::skip_list<IntervalNode<T, Compare>> inverted_lists_;
};

template<typename T, typename C>
IntervalIndexer<T, C>::IntervalIndexer(const std::string& name) : Indexer(name) {
    #define INT32_OFFSET        1
    #define DOUBLE_OFFSET       2
    #define STRING_OFFSET       3
    int32_t flag(0x01);
    flag <<= std::is_same<T, int32_t>() * INT32_OFFSET;
    flag <<= std::is_same<T, double>() * DOUBLE_OFFSET;
    flag <<= std::is_same<T, std::string>() * STRING_OFFSET;
    flag |= INTERVAL_TYPE_MASK;

    if (flag < MIN_INTERVAL_TYPE || flag > MAX_INTERVAL_TYPE) {
        throw std::logic_error("unsupported interval indexer type");
    } else {
        type_ = static_cast<ValueType>(flag);
    }
}

template<typename T, typename C>
IntervalIndexer<T, C>::~IntervalIndexer() {}

// TODO support single value, e.g. age ∈ {18}
template<typename T, typename C>
bool IntervalIndexer<T, C>::ParseTermsFromConjValue(std::vector<Term>& terms, const ConjValue& value) {
    if (type_ == INT32_INTERVAL) {
        for (auto& p : value.int32_intvl()) {
            terms.push_back(Term(name_, p.left(), p.right(), p.flag()));
        }
    }  else if (type_ == DOUBLE_INTERVAL) {
        for (auto& p : value.double_intvl()) {
            terms.push_back(Term(name_, p.left(), p.right(), p.flag()));
        }
    }  else if (type_ == STRING_INTERVAL) {
        for (auto& p : value.string_intvl()) {
            terms.push_back(Term(name_, p.left(), p.right(), p.flag()));
        }
    }
    return true;
}

// TODO support range search like 18 <= age < 20, not only single value
template<typename T, typename C>
const std::list<DocidNode>* IntervalIndexer<T, C>::GetPostingLists(const Term& term) {
    IntervalNode<T> search_node(term, type_);
    if (!search_node) {
        return NULL;
    }
    // 得到实际是交集
    typename goodliffe::skip_list<IntervalNode<T, C>>::iterator iter = inverted_lists_.find(search_node);
    if (iter != inverted_lists_.end()) {
        const InvertedList& li = iter->list();
        return &(li.doc_list());
    } else {
        return NULL;
    }
}

//
// ParseTermsFromConjValue 已经保证term的合法性(即term的类型和T一致)
//
template<typename T, typename C>
bool IntervalIndexer<T, C>::Add(const Term& term, bool is_belong_to, int docid) {
    IntervalNode<T> search_node(term, type_);
    while (search_node) {
        // try to find the the first node whose intersection with term is not empty 
        typename goodliffe::skip_list<IntervalNode<T, C>>::iterator iter = inverted_lists_.find(search_node);
        // empty intersection 
        if (iter != inverted_lists_.end()) {
            // TODO, check...
            search_node.Add(is_belong_to, docid);
            inverted_lists_.insert(search_node);
            break;
        } else {
            Interval<T> left, intersection, right, xleft, xright;
            iter->Slice(search_node, left, intersection, right); 
            search_node.Subtract(*iter, xleft, xright);
            if (!left && !right) {
                // the old is absolutely covered 
                iter->Add(is_belong_to, docid);
            } else {
                InvertedList ilist;
                ilist.Copy(iter->list());
                IntervalNode<T> node(intersection, ilist);
                inverted_lists_.erase(iter);
                node.Add(is_belong_to, docid);
                inverted_lists_.insert(node);

                if (left) {
                    IntervalNode<T> left_node(left, ilist);
                    inverted_lists_.insert(left_node);
                }
                if (right) {
                    IntervalNode<T> right_node(left, ilist);
                    inverted_lists_.insert(right_node);
                }
            }
            if (xleft) {
                IntervalNode<T> xleft_node(xleft);
                xleft_node.Add(is_belong_to, docid);
                inverted_lists_.insert(xleft_node);
            }
            search_node = xright;
        }
    }
    return true;
}

// [10, 18), [20, 30)
template<typename T, typename C>
bool IntervalIndexer<T, C>::Add(const ConjValue& value, bool is_belong_to, int docid) {
    std::vector<Term> terms;
    this->ParseTermsFromConjValue(terms, value);
    for (auto &term : terms) {
        // [10, 18)
        if (!this->Add(term, is_belong_to, docid)) {
            return false;
        }
    }
    return true;
}

} // namespace cloris

#endif // CLORIS_INTERVAL_INDEXER_H_
