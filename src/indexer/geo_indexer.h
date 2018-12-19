//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_GEO_INDEXER_H_
#define CLORIS_GEO_INDEXER_H_

#include <unordered_map>
#include <string>
#include "inverted_list.h"
#include "internal/cloriskip/skip_list.h"
#include "geohash_helper.h"
#include "indexer.h"

namespace cloris {

class GeoNode {
public:
    GeoNode(GeoHashFix52Bits geo_bits) : geo_bits_(geo_bits) {}
    ~GeoNode() {}
    bool operator < (const GeoNode& other) const {
        return (this->geo_bits_ < other.geo_bits());
    }
    bool operator >= (const GeoNode& other) const {
        return !(operator < (other));
    }
    void Add(bool is_belong_to, int docid) { return list_.Add(is_belong_to, docid); }
    const InvertedList& list() const { return list_; }
    GeoHashFix52Bits geo_bits() const { return geo_bits_; }
private:
    GeoNode() = delete;
    GeoHashFix52Bits geo_bits_; 
    InvertedList list_;
};

class GeoIndexer : public Indexer {
    static void ReclaimPostingList(std::list<DocidNode>* lptr);
public:
    GeoIndexer(const std::string& name);
    ~GeoIndexer();
    virtual bool ParseTermsFromConjValue(std::vector<Term>& terms, const ConjValue& value); 
    virtual bool Add(const ConjValue& value, bool is_belong_to, int docid);
    virtual std::list<DocidNode>* GetPostingLists(const Term& term);
    void GetGeoPointsInRange(GeoHashFix52Bits min, GeoHashFix52Bits max, 
            double lon, double lat, double radius, std::list<DocidNode> *lptr);
private:
    bool Add(const Term& term, bool is_belong_to, int docid); 
    void GetMembersOfGeoHashBox(GeoHashBits hash, std::list<DocidNode> *lptr, double lon, double lat, double radius);
    void GetMembersOfAllNeighbors(const GeoHashRadius& n, double lon, double lat, double radius, std::list<DocidNode>* lptr); 
    GeoIndexer() = delete;
    goodliffe::skip_list<GeoNode> inverted_lists_;
};

} // namespace cloris

#endif // CLORIS_GEO_INDEXER_H_
