#ifndef __CAR_BRIDGE_FUNCTION_H
# define __CAR_BRIDGE_FUNCTION_H

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

template<class Argument1, class Argument2, class Result>
struct BinaryFunction {
    typedef Argument1 FirstArgumentType;
    typedef Argument2 SecondArgumentType;

    typedef Result ResultType;
};

template<class T>
struct Less: BinaryFunction<T, T, bool> {
    bool operator()(T const &x, T const &y) const
    {
        return x < y;
    }
};

CAR_BRIDGE_NAMESPACE_END

#endif
