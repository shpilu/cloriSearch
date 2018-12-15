//==============================================================================
// skip_list_detail.h
// Copyright (c) 2011 Pete Goodliffe. All rights reserved.
//==============================================================================

#pragma once

#include <cmath>      // for std::log
#include <cstdlib>    // for std::rand

//==============================================================================

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning (disable : 4068 ) /* disable unknown pragma warnings */
#endif

//==============================================================================
// #pragma mark - internal forward declarations

namespace goodliffe {

/// @internal
/// Internal namespace for impementation of skip list data structure
namespace detail
{
    template <unsigned NumLevels>   class bit_based_skip_list_level_generator;
    template <unsigned NumLevels>   class skip_list_level_generator;
}
}

//==============================================================================
// #pragma mark - diagnostics
//==============================================================================

//#define SKIP_LIST_IMPL_DIAGNOSTICS 1

#if defined(DEBUG) || defined(_DEBUG) || defined(SKIP_LIST_IMPL_DIAGNOSTICS)
    #define SKIP_LIST_DIAGNOSTICS 1
#endif

#ifdef SKIP_LIST_DIAGNOSTICS

    #include <cstdio>

    void pg_assertion_break();
    inline
    void pg_assertion_break() { fprintf(stderr, "**** place a breakpoint at pg_assertion_break to debug\n"); }
    #include <cassert>
    #include <stdio.h>
    #define pg_fail(a)            {fprintf(stderr,"%s:%d: \"%s\"\n", __FILE__, __LINE__, a); /*assert(false);*/ pg_assertion_break(); }
    #define assert_that(a)        {if (!(a)) pg_fail(#a);}
    #define pg_not_implemented_yet() pg_fail("not implemented yet")

#else

    #define pg_fail(a)            
    #define assert_that(a)        
    #define pg_not_implemented_yet() 

#endif


#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
    #include <iostream>
    #define impl_assert_that(a) assert_that(a)
#else
    #define impl_assert_that(a)
#endif


namespace goodliffe {
namespace detail {

template<bool> struct static_assert_that_impl;
template<> struct static_assert_that_impl<true> {};
    
#define static_assert_that(a) \
    {::goodliffe::detail::static_assert_that_impl<a> foo;(void)foo;}

#ifdef SKIP_LIST_IMPL_DIAGNOSTICS
enum
{
    MAGIC_GOOD = 0x01020304,
    MAGIC_BAD  = 0xfefefefe
};
#endif

} // namespace detail
} // namespace goodliffe

//==============================================================================
// #pragma mark - skip_list level generators
//==============================================================================

namespace goodliffe {
namespace detail {

/// Generate a stream of levels, probabilstically chosen.
/// - With a probability of 1/2, return 0.
/// - With 1/4 probability, return 1.
/// - With 1/8 probability, return 2.
/// - And so forth.
template <unsigned NumLevels>
class skip_list_level_generator
{
public:
    static const unsigned num_levels = NumLevels;
    unsigned new_level();
};

template <unsigned NumLevels>
class bit_based_skip_list_level_generator
{
public:
    static const unsigned num_levels = NumLevels;
    unsigned new_level();
};

} // namespace detail
} // namespace goodliffe

//==============================================================================
// #pragma mark - value equivalence based on "less"
//==============================================================================

namespace goodliffe {
namespace detail {

#if 1

template <typename Compare, typename T>
inline
bool equivalent(const T &lhs, const T &rhs, const Compare &less)
    { return !less(lhs, rhs) && !less(rhs, lhs); }

template <typename Compare, typename T>
inline
bool less_or_equal(const T &lhs, const T &rhs, const Compare &less)
    { return !less(rhs, lhs); }

#else

// These "simple" versions are left here for efficiency comparison with
// the versions above.
// There should be no appriciable difference in performance (at least, for
// the built-in types).

template <typename Compare, typename T>
inline
bool equivalent(const T &lhs, const T &rhs, Compare &less)
    { return lhs == rhs; }

template <typename Compare, typename T>
inline
bool less_or_equal(const T &lhs, const T &rhs, Compare &less)
    { return lhs <= rhs; }

#endif

} // namespace detail
} // namespace goodliffe

//==============================================================================
// #pragma mark - skip_list_level_generator
//==============================================================================

namespace goodliffe {
namespace detail {

template <unsigned ML>
inline
unsigned bit_based_skip_list_level_generator<ML>::new_level()
{
    // The number of 1-bits before we encounter the first 0-bit is the level of
    /// the node. Since R is 32-bit, the level can be at most 32.
    assert_that(num_levels < 33);

    unsigned level = 0;
    for (unsigned number = unsigned(rand()); (number & 1) == 1; number >>= 1)
    {
        level++;
    }
    return level;
}

template <unsigned ML>
inline
unsigned skip_list_level_generator<ML>::new_level()
{
    float f = float(std::rand())/float(RAND_MAX);
    unsigned level = unsigned(std::log(f)/std::log(0.5));
    return level < num_levels ? level : num_levels;
}

} // namespace detail
} // namespace goodliffe

//==============================================================================

#ifdef _MSC_VER
#pragma warning( pop )
#endif
