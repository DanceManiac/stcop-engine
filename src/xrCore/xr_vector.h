#pragma once
#include <vector>
#include "xalloc.h"

#define DEF_VECTOR(N, T)\
    using N = xr_vector<T>;\
    using N##_it = N::iterator;

#define DEFINE_VECTOR(T, N, I)\
    using N = xr_vector<T>;\
    using I = N::iterator;

// vector
template <typename T, typename allocator = xalloc<T> >
using xr_vector = class std::vector<T, allocator>;