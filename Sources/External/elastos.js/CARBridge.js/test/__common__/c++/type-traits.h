#ifndef __CAR_BRIDGE_TEST_TYPE_TRAITS_H
# define __CAR_BRIDGE_TEST_TYPE_TRAITS_H

# include "macros.h"



CAR_BRIDGE_TEST_NAMESPACE_BEGIN

template<class T>
struct RemoveConst {
    typedef T Type;
};

template<class T>
struct RemoveConst<T const> {
    typedef T Type;
};


template<class T>
struct RemoveReference {
    typedef T Type;
};

template<class T>
struct RemoveReference<T &> {
    typedef T Type;
};

# if __cplusplus >= 201103L
template<class T>
struct RemoveReference<T &&> {
    typedef T Type;
};

# endif

CAR_BRIDGE_TEST_NAMESPACE_END

#endif
