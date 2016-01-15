
#ifndef __ETL_ITERATOR_BASE_TYPES_H__
#define __ETL_ITERATOR_BASE_TYPES_H__

#include <elastos/utility/etl/etl_def.h>

_ETL_NAMESPACE_BEGIN

struct InputIteratorTag {};
struct OutputIteratorTag {};
struct ForwardIteratorTag : public InputIteratorTag {};
struct BidirectionalIteratorTag : public ForwardIteratorTag {};
struct RandomAccessIteratorTag : public BidirectionalIteratorTag {};

template <typename Category, typename T, typename Distance = ptrdiff_t,
        typename _Pointer = T*, typename _Reference = T&>
struct Iterator
{
    typedef Category    IteratorCategory;
    typedef T           ValueType;
    typedef Distance    DifferenceType;
    typedef _Pointer    Pointer;
    typedef _Reference  Reference;
};

template <typename Iterator>
struct IteratorTraits
{
    typedef typename Iterator::IteratorCategory IteratorCategory;
    typedef typename Iterator::ValueType        ValueType;
    typedef typename Iterator::DifferenceType   DifferenceType;
    typedef typename Iterator::Pointer          Pointer;
    typedef typename Iterator::Reference        Reference;
};

template <typename T>
struct IteratorTraits<T*>
{
    typedef RandomAccessIteratorTag IteratorCategory;
    typedef T                       ValueType;
    typedef ptrdiff_t               DifferenceType;
    typedef T*                      Pointer;
    typedef T&                      Reference;
};

template <typename T>
struct IteratorTraits<const T*>
{
    typedef RandomAccessIteratorTag IteratorCategory;
    typedef T                       ValueType;
    typedef ptrdiff_t               DifferenceType;
    typedef const T*                Pointer;
    typedef const T&                Reference;
};

template <typename Iterator>
inline typename IteratorTraits<Iterator>::IteratorCategory
IteratorCategory(const Iterator&)
{
    return typename IteratorTraits<Iterator>::IteratorCategory();
}

_ETL_NAMESPACE_END

#endif //__ETL_ITERATOR_BASE_TYPES_H__
