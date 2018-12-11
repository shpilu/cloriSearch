// 
// implementaton of singleton pattern
// Copyright (C) 2017 James Wei (weijianlhp@163.com). All rights reserved
//
#ifndef CLORIS_SINGLETON_H_
#define CLORIS_SINGLETON_H_

#include <stdlib.h>
#include <pthread.h>
#include <boost/noncopyable.hpp>

namespace cloris {

template <typename T>
class Singleton : boost::noncopyable {
public:
    static T* instance() {
        pthread_once(&ponce_, &Singleton::Init);
        return value_;
    }
private:
    static void Init() {
        value_ = new T();
        ::atexit(Destroy);
    }

    static void Destroy() {
        delete value_;
    }
private:
    static pthread_once_t ponce_;
    static T* value_;
};

template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;

} // namespace cloris 

#endif // CLORIS_SINGLETON_H_
