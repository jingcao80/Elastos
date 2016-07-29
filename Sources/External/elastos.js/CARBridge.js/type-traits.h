#ifndef __CAR_BRIDGE_TYPE_TRAITS_H
# define __CAR_BRIDGE_TYPE_TRAITS_H

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

template<class T>
struct AddLeftValueReference {
    typedef T &Type;
};

template<class T>
struct AddLeftValueReference<T &> {
    typedef T &Type;
};

template<>
struct AddLeftValueReference<void> {
    typedef void Type;
};

template<>
struct AddLeftValueReference<void const> {
    typedef void const Type;
};

template<class T>
struct RemoveExtent {
    typedef T Type;
};

template<class T>
struct RemoveExtent<T []> {
    typedef T Type;
};

template<class T, size_t n>
struct RemoveExtent<T[n]> {
    typedef T Type;
};

template<class T>
struct RemoveAllExtents {
    typedef T Type;
};

template<class T>
struct RemoveAllExtents<T []> {
    typedef typename RemoveAllExtents<T>::Type Type;
};

template<class T, size_t n>
struct RemoveAllExtents<T[n]> {
    typedef typename RemoveAllExtents<T>::Type Type;
};

CAR_BRIDGE_NAMESPACE_END

#endif
