//
// An abstract description of Interval 
// iterval indexer main class definition
// A generic interval indexer implementation, e.g. age ∈ [18, 25]
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_INTERVAL_H_ 
#define CLORIS_INTERVAL_H_

#include <unistd.h>
#include <functional>

// 
// #define INTERVAL_OPEN           0x00000000
// #define INTERVAL_OPEN_CLOSE     0x00000001
// #define INTERVAL_CLOSE_OPEN     0x00000002
// #define INTERVAL_CLOSE          0x00000003
// #define INTERVAL_EMPTY          0x00000004
// 
// #define INTERVAL_LEFT_MASK      0x00000002
// #define INTERVAL_RIGHT_MASK     0x00000001 

// NEW ----
#define INTERVAL_EMPTY          0x00000008

#define INTERVAL_RIGHT_OPEN     0x00000001 // )
#define INTERVAL_CLOSE_MASK     0x00000002 // [ or ]
#define INTERVAL_LEFT_OPEN      0x00000004 // (

namespace cloris {

template<typename T>
struct BoundaryPoint {
    BoundaryPoint() : flag(INTERVAL_EMPTY) {}
    bool operator < (const BoundaryPoint& other) { 
        if (flag & INTERVAL_EMPTY) {
            return (other.flag & INTERVAL_EMPTY) ? false : true;
        } else {
            if (other.flag & INTERVAL_EMPTY) {
                retruen false;
            } else {
                return (value < other.value) || ((value == other.value) && (flag < other.flag)); 
            }
        }
    }
    //
    // weight: ( ==> 4, [ ==> 2, ] ==> 2, ) ==> 1
    //
    // 
    BoundaryPoint get_inverted(Dir dir) {
        uint8_t flag;
        if (flag & INTERVAL_CLOSE_MASK) {
            flag = (dir == RIGHT) ? INTERVAL_LEFT_OPEN : INTERVAL_RIGHT_OPEN;
        } else {
            flag = INTERVAL_CLOSE_MASK;
        }
        return std::move(BoundaryPoint(value, flag));
    }
    T value;
    uint8_t flag;
};

template <typename T, typename Compare = std::less<T>>
class Interval {
    typedef Interval<T, Compare> SelfType;
    typedef BoundaryPoint<T> Point; 
public:
    SelfType() {}
    ~SelfType() {}
    MakeInterval(const Point& left, const Point& right) : left_(left), right_(right) {}
    inline uint8_t flag() const { return left_.flag | right_.flag(); }
    bool operator() { return !(flag() & INTERVAL_EMPTY); }
    void Subtract(const SelfType& target, SelfType& left_result, SelfType& right_result);
    void Intersect(const SelfType& target, SelfType& result); 
    inline const Point& left() const { return left_; }
    inline const Point& right() const { return right_; }
    inline bool is_empty() const { return flag() & INTERVAL_EMPTY; }
    inline void set_empty() const { 
        left_.flag |= INTERVAL_EMPTY; 
        right_.flag |= INTERVAL_EMPTY; 
    }
private:
    Point left_;
    Point right_;
};

template <typename T, typename Compare>
void Interval::Subtract(const SelfType& target, SelfType& left_result, SelfType& right_result) {
    if (this->is_empty()) {
        left_result.set_empty();
        right_result.set_empty();
        return;
    }
    // we treat empty interval as the smallest interval
    if (target->is_empty()) {
        left_result.set_empty();
        right_result = *this; 
        return;
    }

    //
    // logic table:
    // A. b.left <= a.left, b.right < a.left | s.left = EMPTY; s.right = a | 
    // B. b.left <= a.left, a.left <= b.right < a.right | s.left = EMPTY; s.right = (b.right, a.right)
    // C. b.left <= a.left, b.right >= a.right | s.left = EMPTY; s.right = EMPTY |
    // D. a.left < b.left, b.right < a.right | s.left = (a.left, b.left), s.right = (b.right, a.right)
    // E. a.left < b.left, b.right >= a.right , b.left <= a.right | s.left = (a.left, b.left), s.right = EMPTY;
    // F. b.left > a.right | s.left = (a.left, a.right), s.right = EMPTY 
    //
    if (target.left() < left()) {
        left_result.set_empty();
        // case A
        if (target.right() < left()) {
            right_result = *this;
        } else {
            // case B
            if (target.right() < right()) {
                right_result = MakeInterval(target.right().get_inverted(DIR::Right), right());
            } else {
                // case C
                right_result.set_empty();
            }
        }
    } else {
        // case D
        if (target.right() < right()) {
            left_result = MakeInterval(left(), target.left().get_inverted(DIR::Left));
            right_result = MakeInterval(target.right().get_inverted(DIR::Right), right());
        } else {
            // case E
            if (b.left() <= right()) {
                left_result = MakeInterval(left(), target.left().get_inverted(DIR::Left));
                right_result.set_empty();
            } else {
                // case F
                left_result = *this;
                right_result.set_empty();
            }
        }
    }
}

template <typename T, typename Compare>
void Interval::Intersect(const SelfType& target, SelfType& result) {
    if (is_empty() || target.is_empty()) {
        result.set_empty();
        return;
    }
    // 
    // logic table:
    // A. b.left <= a.left, b.right < a.left | s = EMPTY
    // B. b.left <= a.left, a.left <= b.right < a.right | s = (a.left, b.right)
    // C. b.left <= a.left, b.right >= a.right | s = a
    // D. a.left < b.left, b.right < a.right | s = b
    // E. a.left < b.left, b.right >= a.right , b.left <= a.right | s = (b.left, a.right)
    // F. b.left > a.right | EMPTY 
    if (target.left() < left()) {
        // case A
        if (target.right() < left()) {
            result.set_empty();
        } else {
            // case B
            if (target.right() < right()) {
                result = MakeInterval(left(), target.right());
            } else {
                // case C
                result = *this;
            }
        }
    } else {
        // case D
        if (target.right() < right()) {
            result = target;
        } else {
            // case E
            if (b.left() <= right()) {
                result = MakeInterval(target.left(), right());
            } else {
                // case F
                result.set_empty();
            }
        }
    }
}

} // namespace cloris

#endif // CLORIS_INTERVAL_H_
