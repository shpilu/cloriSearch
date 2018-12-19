//
// indexer main class definition
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#include "geohash.h"
#include "internal/log.h"
#include "geo_indexer.h"

namespace cloris {

GeoIndexer::GeoIndexer(const std::string &name) : Indexer(name) {
    type_ = ValueType::GEORANGE;
    reclaim_handler_ = GeoIndexer::ReclaimPostingList;
}

GeoIndexer::~GeoIndexer() {
}

// |   double   |   double  | int32_t | 
// | longitude | latitude | radius  |
bool GeoIndexer::Add(const Term& term, bool is_belong_to, int docid) {
    /* Turn the coordinates into the score of the element. */
    GeoHashBits hash;
    geohashEncodeWGS84(term.longitude(), term.latitude(), GEO_STEP_MAX, &hash);
    GeoHashFix52Bits bits = geohashAlign52Bits(hash);
    // add to skip_list
    GeoNode node(bits);
    typename goodliffe::skip_list<GeoNode>::iterator iter = inverted_lists_.find(node);
    // not found
    if (iter == inverted_lists_.end()) {
        node.Add(is_belong_to, docid);
        inverted_lists_.insert(node);
    } else {
        iter->Add(is_belong_to, docid);
    }
    return true; 
}

bool GeoIndexer::ParseTermsFromConjValue(std::vector<Term>& terms, const ConjValue& value) {
    if (value.has_geo()) {
        Term tm(GeoRange(value.geo().lon(), value.geo().lat()));
       // Term tm(range); 
        terms.push_back(tm);
    }
    return true;
}

bool GeoIndexer::Add(const ConjValue& value, bool is_belong_to, int docid) {
    std::vector<Term> terms;
    this->ParseTermsFromConjValue(terms, value);
    for (auto &term : terms) {
        if (!this->Add(term, is_belong_to, docid)) {
            return false;
        }
    }
    return true;
}

/* ====================================================================
 * Helpers
 * ==================================================================== */
int decodeGeohash(double bits, double *xy) {
    GeoHashBits hash = { .bits = (uint64_t)bits, .step = GEO_STEP_MAX };
    return geohashDecodeToLongLatWGS84(hash, xy);
}

void geoAppendIfWithinRadius(std::list<DocidNode>* lptr, double lon, double lat, double radius, const GeoNode& cur_node) {
    double distance, xy[2];
    double score = cur_node.geo_bits();
    if (!decodeGeohash(score, xy)) {
        return;
    }
    if (!geohashGetDistanceIfInRadiusWGS84(lon,lat, xy[0], xy[1], radius, &distance)) {
        return;
    }
    for (auto& p : cur_node.list().doc_list()) {
        lptr->push_back(p);
    } 
    return;
}

/* Query a Redis sorted set to extract all the elements between 'min' and
 * 'max', appending them into the array of geoPoint structures 'gparray'.
 * The command returns the number of elements added to the array.
 *
 * Elements which are farest than 'radius' from the specified 'x' and 'y'
 * coordinates are not included.
 *
 * The ability of this function to append to an existing set of points is
 * important for good performances because querying by radius is performed
 * using multiple queries to the sorted set, that we later need to sort
 * via qsort. Similarly we need to be able to reject points outside the search
 * radius area ASAP in order to allocate and process more points than needed. */
void GeoIndexer::GetGeoPointsInRange(GeoHashFix52Bits min, GeoHashFix52Bits max, 
        double lon, double lat, double radius, std::list<DocidNode> *lptr) {
    GeoNode min_node(min);
    GeoNode max_node(max);
    typename goodliffe::skip_list<GeoNode>::iterator iter = inverted_lists_.find_first_in_range(min_node, max_node);
    
    while (iter != this->inverted_lists_.end()) {
        if (*iter >= max_node) {
            break;
        }
        ++iter;
        geoAppendIfWithinRadius(lptr, lon, lat, radius, iter->geo_bits());
    }
}

/* Compute the sorted set scores min (inclusive), max (exclusive) we should
 * query in order to retrieve all the elements inside the specified area
 * 'hash'. The two scores are returned by reference in *min and *max. */
void scoresOfGeoHashBox(GeoHashBits hash, GeoHashFix52Bits *min, GeoHashFix52Bits *max) {                                                 
    /* We want to compute the sorted set scores that will include all the
     * elements inside the specified Geohash 'hash', which has as many
     * bits as specified by hash.step * 2.
     *
     * So if step is, for example, 3, and the hash value in binary
     * is 101010, since our score is 52 bits we want every element which
     * is in binary: 101010?????????????????????????????????????????????
     * Where ? can be 0 or 1.
     *
     * To get the min score we just use the initial hash value left
     * shifted enough to get the 52 bit value. Later we increment the
     * 6 bit prefis (see the hash.bits++ statement), and get the new
     * prefix: 101011, which we align again to 52 bits to get the maximum
     * value (which is excluded from the search). So we get everything
     * between the two following scores (represented in binary):
     *
     * 1010100000000000000000000000000000000000000000000000 (included)
     * and
     * 1010110000000000000000000000000000000000000000000000 (excluded).
     */
    *min = geohashAlign52Bits(hash);
    hash.bits++;
    *max = geohashAlign52Bits(hash);
}

/* Obtain all members between the min/max of this geohash bounding box.
 * Populate a geoArray of GeoPoints by calling GetGeoPointsInRange().
 * Return the number of points added to the array. */
void GeoIndexer::GetMembersOfGeoHashBox(GeoHashBits hash, std::list<DocidNode> *lptr, double lon, double lat, double radius) {
    GeoHashFix52Bits min, max;
    scoresOfGeoHashBox(hash,&min,&max);
    GetGeoPointsInRange(min, max, lon, lat, radius, lptr);
}


/* Search all eight neighbors + self geohash box */
void GeoIndexer::GetMembersOfAllNeighbors(GeoHashRadius n, double lon, double lat, double radius, std::list<DocidNode>* lptr) {
    GeoHashBits neighbors[9];
    unsigned int i, last_processed = 0;

    neighbors[0] = n.hash;
    neighbors[1] = n.neighbors.north;
    neighbors[2] = n.neighbors.south;
    neighbors[3] = n.neighbors.east;
    neighbors[4] = n.neighbors.west;
    neighbors[5] = n.neighbors.north_east;
    neighbors[6] = n.neighbors.north_west;
    neighbors[7] = n.neighbors.south_east;
    neighbors[8] = n.neighbors.south_west;

    /* For each neighbor (*and* our own hashbox), get all the matching
     * members and add them to the potential result list. */
    for (i = 0; i < sizeof(neighbors) / sizeof(*neighbors); i++) {
        if (HASHISZERO(neighbors[i])) {
            continue;
        }
        /* When a huge Radius (in the 5000 km range or more) is used,
         * adjacent neighbors can be the same, leading to duplicated
         * elements. Skip every range which is the same as the one
         * processed previously. */
        if (last_processed &&
            neighbors[i].bits == neighbors[last_processed].bits &&
            neighbors[i].step == neighbors[last_processed].step)
        {
            continue;
        }
        this->GetMembersOfGeoHashBox(neighbors[i], lptr, lon, lat, radius);
        last_processed = i;
    }
}

void GeoIndexer::ReclaimPostingList(std::list<DocidNode>* lptr) {
    delete lptr;
}

std::list<DocidNode>* GeoIndexer::GetPostingLists(const Term& term) {
    if (term.type() != ValueType::GEORANGE) {
        cLog(ERROR, "[geo_indexer] (GetPostingLists) bad term type");
        return NULL;
    }
    double longitude = term.longitude();
    double latitude  = term.latitude();
    double radius_mters   = term.radius();
    GeoHashRadius georadius = geohashGetAreasByRadiusWGS84(longitude, latitude, radius_mters);
    /* Search the skip_list for all matching points */
    std::list<DocidNode>* lptr = new (std::list<DocidNode>);
    this->GetMembersOfAllNeighbors(georadius, longitude, latitude, radius_mters, lptr); 
    return lptr;
}

} // namespace cloris
