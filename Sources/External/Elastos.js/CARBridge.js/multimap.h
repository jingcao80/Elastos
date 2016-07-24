#ifndef __CAR_BRIDGE_MULTIMAP_H
# define __CAR_BRIDGE_MULTIMAP_H

# ifndef __USE_MALLOC
#  define __USE_MALLOC
# endif
# include <elastos/utility/etl/Map.h>

# include "macros.h"

# include "function.h"
# include "pair.h"



CAR_BRIDGE_NAMESPACE_BEGIN

template<class Key, class Value, class Compare = Less<Key>, class Alloc = _ETL Allocator<_ETL Pair<Key const, Value>>>
class Multimap: public _ETL Multimap<Key, Value, Compare, Alloc> {
public:
    Multimap(void): _ETL Multimap<Key, Value, Compare, Alloc>()
    {}

    using typename _ETL Multimap<Key, Value, Compare, Alloc>::AllocatorType;

    explicit Multimap(Compare const &compare, AllocatorType const &allocator = AllocatorType()):
        _ETL Multimap<Key, Value>(compare, allocator)
    {}

    template<class InputIterator>
    Multimap(InputIterator first, InputIterator last): _ETL Multimap<Key, Value, Compare, Alloc>(first, last)
    {}

    template<class InputIterator>
    Multimap(InputIterator first, InputIterator last,
            Compare const &compare,
            AllocatorType const &allocator = AllocatorType()):
        _ETL Multimap<Key, Value, Compare, Alloc>(first, last, compare, allocator)
    {}

    Multimap(Multimap<Key, Value, Compare, Alloc> const &multimap): _ETL Multimap<Key, Value, Compare, Alloc>(multimap)
    {}

    using typename _ETL Multimap<Key, Value, Compare, Alloc>::Iterator;

    using typename _ETL Multimap<Key, Value, Compare, Alloc>::ValueType;

    using _ETL Multimap<Key, Value, Compare, Alloc>::insert;

    Iterator Insert(ValueType const &value)
    {
        return insert(value);
    }

    Iterator Insert(Iterator position, ValueType const &value)
    {
        return insert(position, value);
    }

    template<class InputIterator>
    void Insert(InputIterator first, InputIterator last)
    {
        return insert(first, last);
    }

    using typename _ETL Multimap<Key, Value, Compare, Alloc>::KeyType;

    using _ETL Multimap<Key, Value, Compare, Alloc>::GetEqualRange;

    Pair<Iterator, Iterator> EqualRange(KeyType const &key)
    {
        return GetEqualRange(key);
    }

    using typename _ETL Multimap<Key, Value, Compare, Alloc>::ConstIterator;

    Pair<ConstIterator, ConstIterator> EqualRange(KeyType const &key) const
    {
        return GetEqualRange(key);
    }
};

CAR_BRIDGE_NAMESPACE_END

#endif
