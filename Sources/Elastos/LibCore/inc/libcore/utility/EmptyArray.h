#ifndef __LIBCORE_UTILITY_EMPTY_ARRAY_H__
#define __LIBCORE_UTILITY_EMPTY_ARRAY_H__

#include <elastos/core/Object.h>

using Elastos::Core::IThrowable;
using Elastos::Core::IStackTraceElement;

namespace Libcore {
namespace Utility {

class ECO_PUBLIC EmptyArray
{
public:
    static const AutoPtr<ArrayOf<Boolean> > BOOLEAN;
    static const AutoPtr<ArrayOf<Byte> > BYTE;
    static const AutoPtr<ArrayOf<Char32> > CHAR32;
    static const AutoPtr<ArrayOf<Double> > DOUBLE;
    static const AutoPtr<ArrayOf<Float> > FLOAT;
    static const AutoPtr<ArrayOf<Int32> > INT32;
    static const AutoPtr<ArrayOf<Int64> > INT64;
    static const AutoPtr<ArrayOf<String> > STRING;

    static const AutoPtr<ArrayOf<IInterface*> > OBJECT;
    static const AutoPtr<ArrayOf<IThrowable*> > THROWABLE;
    static const AutoPtr<ArrayOf<IStackTraceElement*> > STACK_TRACE_ELEMENT;

private:
    ECO_LOCAL EmptyArray();
    ECO_LOCAL EmptyArray(const EmptyArray&);
};

} // namespace Utility
} // namespace Libcore

#endif //__LIBCORE_UTILITY_EMPTY_ARRAY_H__