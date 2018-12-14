//
// iterval indexer main class definition
// A generic interval indexer implementation, e.g. age ∈ [18, 25]
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//
//
//
// TODO @James Wei
// 1.  It's bad efficient to 目前跳跃表的插入效率不高, just rewrite skip_list 
//
//      2. 插入可能导致区间合并: e.g. Interval(10, 18] -- Docid(1, 2, 3, 4), Interval(18, 22] -- Docid(1, 2, 3) 在插入Interval(10, 22] -- Docid(4) 时应该合并区间为(10, 22] -- Docid(1, 2, 3, 4), 但目前没有合并区间
//
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
#include "third_party/skip_list/skip_list.h"
#include "interval.h"

#define INF 
#define SUP

namespace cloris {

template <typename T, typename Comp = std::less<T>>
class IntervalNode : public Interval<T> {
public:
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
    IntervalNode(Term& term); 
    Add(bool is_belong_to, int docid) { list_.Add(is_belong_to, docid); }
    Interval<T> GetConjunction(IntervalNode<T>& other);
private:
    InvertedList list_;
}

// TODO @James Wei
// Add compile_time type check
// for string type
template<typename T, typename A>
IntervalNode::IntervalNode(Term& term) {
    if (!(type_ & term.type() & BASIC_TYPE_MASK)) {
        type_ = IntervalType::BAD;
        return;
    }
    // if term is a interval type
    if ((type_ & term.type()) & INTERVAL_TYPE_MASK) {
        flag_ = term.flag(); 
        left_ = *reinterpret_cast<T*>(term.left());
        right_ = *reinterpret_cast<T*>(term.right());
    } else {
        // term is not a interval type, extend it to [X, X]
        left_ = *reinterpret_cast<T*>(term.value()[0]);
        right_ = *reinterpret_cast<T*>(term.value()[0]);
        flag_ = IntervalType::CLOSE;
    }
}

// special for string type
template<std::string, typename A>
IntervalNode::IntervalNode(Term& term) {
    if (!(type_ & term.type() & BASIC_TYPE_MASK)) {
        type_ = IntervalType::BAD;
        return;
    }
    // if term is a interval type
    if ((type_ & term.type()) & INTERVAL_TYPE_MASK) {
        flag_ = term.flag(); 
        size_t left_size;
        size_t right_size;
        void *lptr = term.left(&left_size);
        void *rptr = term.right(&right_size);
        left_.assign(reinterpret_cast<char*>(lptr), left_size);
        right_.assign(reinterpret_cast<char*>(rptr), right_size);
    } else {
        // if term is not a interval type, extend it to [X, X]
        left_ = term.value();
        right_ = term.value();
        flag_ = IntervalType::CLOSE;
    }
}

template<typename T, typename Compare = std::less<T>>
class IntervalIndexer : public Indexer {
public:
    IntervalIndexer(const std::string& name);
    ~IntervalIndexer();
    bool ParseTermsFromConjValue(std::vector<Term>& terms, const ConjValue& value);
    virtual bool Add(const ConjValue& value, bool is_belong_to, int docid);
    virtual const std::list<DocidNode>* GetPostingLists(const Term& term);
private:
    goodliffe::skip_list<IntervalNode<T, Compare>> inverted_lists_;
};

template<typename T, typename C>
IntervalIndexer<T, C>::IntervalIndexer(const std::string& name) 
    : name_(name) {
    if (std::is_same(T, int32_t)) {
        type_ = INT32_INTERVAL;
    } else if (std::is_same(T, float)) {
        type_ = DOUBLE_INTERVAL;
    } else if (std::is_same(T, std::string)) {
        type_ == STRING_INTERVAL;
    } else {
        throw std::logic_error("unsupported interval indexer type");
    }
}

template<typename T, typename C>
IntervalIndexer<T, C>::~IntervalIndexer() {}

// TODO
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

// DDX
template<typename T, typename C>
const std::list<DocidNode>* IntervalIndexer<T, C>::GetPostingLists(const Term& term) {
    IntervalNode<T> search_node(term);
    if (!search_node()) {
        return NULL;
    }
    // 得到实际是交集
    goodliffe::skip_list<IntervalNode<T>>::iterator iter = inverted_lists_.find(search_node);
    if (iter) {
        return iter->posting_list();
    } else {
        return NULL;
    }
}

//
// ParseTermsFromConjValue 已经保证term的合法性(即term的类型和T一致)
//
template<typename T, typename C>
bool IntervalIndexer<T, C>::Add(const Term& term, bool is_belong_to, int docid) {
    IntervalNode<T> search_node(term);
    // 找到第一个有交集的点
    while (search_node()) {
        goodliffe::skip_list<IntervalNode<T>, C>::iterator iter = inverted_lists_.find(search_node);
        // 没有交集
        if (!iter) {
            // TODO, check...
            search_node.Add(is_belong_to, docid);
            inverted_lists_.insert(search_node);
        } else {
            // old, new,
            // 计算交集
            Interval<T> intvl_conj = iter->GetConjunction(search_node);
            std::pair<Interval<T>, Interval<T>> ivl = iter->CutSubset(intvl_conj); // left
            if (!ival.first() && !ival.second()) {
                // 新的完全包含旧的
                iter->Add(is_belong_to, docid);
            } else {
                IntervalNode<T> node(intvl_conj, *iter);
                inverted_lists_.erase(iter);
                
                node.Add(is_belong_to, docid);

                inverted_lists_.insert(node);

                if (ival.first()) {
                    ival.first.copy(*iter);
                    inverted_lists_.insert(ival.first);
                }
                if (ival.second()) {
                    ival.second.copy(*iter)
                    inverted_lists_.insert(ival.second);
                }
            }
            std::pair<Interval<T>, Interval<T>> xival = search_node.CutSubset(intvl_conj);
            if (xival > ZERO) {
                // 插左结点
            }
            search_node = right(search_node - iter);
            goto next_loop;
            
        }
    }
    if (search_node > ZERO) {
        inverted_lists_.insert(is_belong_to, docid);
    }
}

// [10, 18), [20, 30)
template<typename T, typename C>
bool IntervalIndexer<T, C>::Add(const ConjValue& value, bool is_belong_to, int docid) {
    std::vector<Term> terms;
    this->ParseTermsFromConjValue(terms, value);
    for (auto &term : terms) {
        // [10, 18)
        = this->Add(term, is_belong_to, docid);
    }
}

} // namespace cloris

#endif // CLORIS_INTERVAL_INDEXER_H_
