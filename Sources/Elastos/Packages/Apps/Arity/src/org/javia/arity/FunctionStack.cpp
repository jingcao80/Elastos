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

#include "org/javia/arity/FunctionStack.h"

namespace Org {
namespace Javia {
namespace Arity {

FunctionStack::FunctionStack()
    : mSize(0)
{
    mData = ArrayOf<IFunction*>::Alloc(8);
}

void FunctionStack::Clear()
{
    mSize = 0;
}

void FunctionStack::Push(
    /* [in] */ IFunction* b)
{
    if (mSize >= mData->GetLength()) {
        AutoPtr<ArrayOf<IFunction*> > newData = ArrayOf<IFunction*>::Alloc(mData->GetLength() << 1);
        newData->Copy(0, mData, 0, mData->GetLength());
        mData = newData;
    }
    mData->Set(mSize++, b);
}

AutoPtr<IFunction> FunctionStack::Pop()
{
    return (*mData)[--mSize];
}

AutoPtr<ArrayOf<IFunction*> > FunctionStack::ToArray()
{
    AutoPtr<ArrayOf<IFunction*> > trimmed = ArrayOf<IFunction*>::Alloc(mSize);
    trimmed->Copy(0, mData, 0, mSize);
    return trimmed;
}

} // namespace Arity
} // namespace Javia
} // namespace Org