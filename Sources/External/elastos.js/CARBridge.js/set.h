#ifndef __CAR_BRIDGE_SET_H
# define __CAR_BRIDGE_SET_H

# ifndef __USE_MALLOC
#  define __USE_MALLOC
# endif
# include <elastos/utility/etl/Set.h>

# include "macros.h"

# include "function.h"



CAR_BRIDGE_NAMESPACE_BEGIN

template<class Key, class Compare = Less<Key>, class Alloc = _ETL Allocator<Key>>
class Set: public _ETL Set<Key, Compare, Alloc> {
public:
    using typename _ETL Set<Key, Compare, Alloc>::SizeType;

    using typename _ETL Set<Key, Compare, Alloc>::KeyType;

    using _ETL Set<Key, Compare, Alloc>::GetCount;

    SizeType Count(KeyType const &key) const
    {
        return GetCount(key);
    }
};

CAR_BRIDGE_NAMESPACE_END

#endif
