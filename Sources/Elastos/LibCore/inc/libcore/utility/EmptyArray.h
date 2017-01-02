//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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