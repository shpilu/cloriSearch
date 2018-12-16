//
// cloriSearch core implementation
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include "query.h"

namespace cloris {

Term& Query::operator[](const std::string& key) {
    if (terms_.find(key) != terms_.end()) {
        return terms_.at(key);
    } else {
        Term term(key);
        terms_.insert(std::pair<std::string, Term>(key, term));
        return terms_.at(key); 
    }
}

// unsafe
Term& Query::at(const std::string& key) {
    return terms_.at(key);
}

void Query::Append(const Term& term) {
    terms_.insert(std::pair<std::string, Term>(term.name(), term));
}

} // namepace cloris
