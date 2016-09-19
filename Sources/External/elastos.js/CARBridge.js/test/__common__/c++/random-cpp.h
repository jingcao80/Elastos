#ifndef __CAR_BRIDGE_TEST_RANDOM_CPP_H
# define __CAR_BRIDGE_TEST_RANDOM_CPP_H

# include <stddef.h>

# include <elastos.h>

# include "macros.h"



CAR_BRIDGE_TEST_NAMESPACE_BEGIN

template<class T>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<T> > RandomArrayOf(void)
{
}

template<class T, size_t L>
T (*RandomCArrayOf(void))[L]
{
}

CAR_BRIDGE_TEST_NAMESPACE_END

#endif
