//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_INDEXER_H_
#define CLORIS_INDEXER_H_

namespace cloris {

class Indexer {
public:
    virtual ~Indexer();
    virtual Add(AttributeValue& value, int docid);
protected:
    AttributeTyle type_;

private:

};

} // namespace cloris

#endif // CLORIS_INDEXER_H_

