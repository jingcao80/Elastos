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