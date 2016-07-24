#ifndef __CAR_BRIDGE_MAP_H
# define __CAR_BRIDGE_MAP_H

# ifndef __USE_MALLOC
#  define __USE_MALLOC
# endif
# include <elastos/utility/etl/Map.h>

# include "macros.h"

# include "function.h"



CAR_BRIDGE_NAMESPACE_BEGIN

template<class Key, class Value, class Compare = Less<Key>, class Alloc = _ETL Allocator<_ETL Pair<Key const, Value>>>
class Map: public _ETL Map<Key, Value, Compare, Alloc> {
public:
    Map(void): _ETL Map<Key, Value, Compare, Alloc>()
    {}

    using typename _ETL Map<Key, Value, Compare, Alloc>::AllocatorType;

    explicit Map(Compare const &compare, AllocatorType const &allocator = AllocatorType()):
        _ETL Map<Key, Value, Compare, Alloc>(compare, allocator)
    {}

    template<class InputIterator>
    Map(InputIterator first, InputIterator last): _ETL Map<Key, Value, Compare, Alloc>(first, last)
    {}

    template<class InputIterator>
    Map(InputIterator first, InputIterator last,
            Compare const &compare,
            AllocatorType const &allocator = AllocatorType()):
        _ETL Map<Key, Value, Compare, Alloc>(first, last, compare, allocator)
    {}

    Map(Map<Key, Value, Compare, Alloc> const &map): _ETL Map<Key, Value, Compare, Alloc>(map)
    {}

    using typename _ETL Map<Key, Value, Compare, Alloc>::SizeType;

    using typename _ETL Map<Key, Value, Compare, Alloc>::KeyType;

    using _ETL Map<Key, Value, Compare, Alloc>::GetCount;

    SizeType Count(KeyType const &key) const
    {
        return GetCount(key);
    }
};

CAR_BRIDGE_NAMESPACE_END

#endif
