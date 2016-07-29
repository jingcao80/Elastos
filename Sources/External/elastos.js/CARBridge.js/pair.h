#ifndef __CAR_BRIDGE_PAIR_H
# define __CAR_BRIDGE_PAIR_H

# ifndef __USE_MALLOC
#  define __USE_MALLOC
# endif
# include <elastos/utility/etl/Pair.h>

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

template<class First, class Second>
using Pair = _ETL Pair<First, Second>;

CAR_BRIDGE_NAMESPACE_END

# define first mFirst
# define second mSecond

#endif
