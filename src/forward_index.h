//
// inverted index main class definition
// InvertedIndex is the manager class of inverted index
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_FORWARD_INDEX_H_
#define CLORIS_FORWARD_INDEX_H_

namespace cloris {

class ForwardIndex {
public:
    ForwardIndex() { }
    ~ForwardIndex() { }

    bool Init();

private:
    int pos_;
    // *p...
};

} // namespace cloris

#endif // CLORIS_FORWARD_INDEX_H_ 
