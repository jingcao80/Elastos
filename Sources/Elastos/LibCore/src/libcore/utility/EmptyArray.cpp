#include "EmptyArray.h"

namespace Libcore {
namespace Utility {


const AutoPtr<ArrayOf<Boolean> > EmptyArray::BOOLEAN = ArrayOf<Boolean>::Alloc(0);
const AutoPtr<ArrayOf<Byte> > EmptyArray::BYTE = ArrayOf<Byte>::Alloc(0);
const AutoPtr<ArrayOf<Char32> > EmptyArray::CHAR32 = ArrayOf<Char32>::Alloc(0);
const AutoPtr<ArrayOf<Double> > EmptyArray::DOUBLE = ArrayOf<Double>::Alloc(0);
const AutoPtr<ArrayOf<Float> > EmptyArray::FLOAT = ArrayOf<Float>::Alloc(0);
const AutoPtr<ArrayOf<Int32> > EmptyArray::INT32 = ArrayOf<Int32>::Alloc(0);
const AutoPtr<ArrayOf<Int64> > EmptyArray::INT64 = ArrayOf<Int64>::Alloc(0);
const AutoPtr<ArrayOf<String> > EmptyArray::STRING = ArrayOf<String>::Alloc(0);

const AutoPtr<ArrayOf<IInterface*> > EmptyArray::OBJECT  = ArrayOf<IInterface*>::Alloc(0);
const AutoPtr<ArrayOf<IThrowable*> > EmptyArray::THROWABLE = ArrayOf<IThrowable*>::Alloc(0);
const AutoPtr<ArrayOf<IStackTraceElement*> > EmptyArray::STACK_TRACE_ELEMENT = ArrayOf<IStackTraceElement*>::Alloc(0);

// public static final Class<?>[] CLASS = new Class[0];
// public static final java.lang.reflect.Type[] TYPE = new java.lang.reflect.Type[0];
// public static final java.lang.reflect.TypeVariable[] TYPE_VARIABLE =
//     new java.lang.reflect.TypeVariable[0];

} // namespace Utility
} // namespace Libcore