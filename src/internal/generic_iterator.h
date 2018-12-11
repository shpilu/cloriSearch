// 
// a simple implementaton of generic iterator (not really, just support MAP type now)
// Copyright (C) 2017 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_GENERIC_ITERATOR_H_
#define CLORIS_GENERIC_ITERATOR_H_

namespace cloris {
// TODO
typedef std::map<std::string, Term> TermNodes;
template <bool Const> struct SelectIfImpl { template <typename T1, typename T2> struct Apply { typedef T1 Type; }; };
template <> struct SelectIfImpl<false> { template <typename T1, typename T2> struct Apply { typedef T2 Type; }; };
template <bool Const, typename T1, typename T2> struct SelectIfCond : SelectIfImpl<Const>::template Apply<T1, T2> {};
template <bool Const, typename T> struct MaybeAddConst : SelectIfCond<Const, const T, T> {};

template <bool Const, typename TypeName>
class GenericIterator {
    typedef typename MaybeAddConst<Const, TypeName>::Type ValueType;
    typedef typename SelectIfCond<Const, TermNodes::const_iterator, TermNodes::iterator>::Type IteratorType;
public:
    typedef GenericIterator Iterator;
    typedef GenericIterator<true, TypeName> ConstIterator;
    typedef GenericIterator<false, TypeName> NonConstIterator;
    GenericIterator() : current_() {}
    GenericIterator(IteratorType iter) : current_(iter) {}

    Iterator& operator++() { ++current_; return *this; }
    Iterator& operator--() { --current_; return *this; }
    ValueType& operator*() const { return current_->second; }
    ValueType* operator->() const { return &current_->second; }
    Iterator& operator=(const NonConstIterator& that) { current_ = that.current_; return *this; }
    bool   operator!=(GenericIterator that) const { return current_ != that.current_; }
private:
    IteratorType current_;
};

} // namespace cloris

#endif // CLORIS_GENERIC_ITERATOR_H_
