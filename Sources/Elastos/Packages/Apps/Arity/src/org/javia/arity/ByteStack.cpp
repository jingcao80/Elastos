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

#include "org/javia/arity/ByteStack.h"

namespace Org {
namespace Javia {
namespace Arity {

ByteStack::ByteStack()
    : mSize(0)
{
    mData = ArrayOf<Byte>::Alloc(8);
}

void ByteStack::Clear()
{
    mSize = 0;
}

void ByteStack::Push(
    /* [in] */ Byte b)
{
    if (mSize >= mData->GetLength()) {
        AutoPtr<ArrayOf<Byte> > newData = ArrayOf<Byte>::Alloc(mData->GetLength() << 1);
        newData->Copy(0, mData, 0, mData->GetLength());
        mData = newData;
    }
    (*mData)[mSize++] = b;
}

Byte ByteStack::Pop()
{
    return (*mData)[--mSize];
}

AutoPtr<ArrayOf<Byte> > ByteStack::ToArray()
{
    AutoPtr<ArrayOf<Byte> > trimmed = ArrayOf<Byte>::Alloc(mSize);
    trimmed->Copy(0, mData, 0, mSize);
    return trimmed;
}

} // namespace Arity
} // namespace Javia
} // namespace Org