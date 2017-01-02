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

#include "elastos/io/NIOAccess.h"
#include "elastos/io/Buffer.h"

namespace Elastos {
namespace IO {

ECode NIOAccess::GetBasePointer(
    /* [in] */ IBuffer* b,
    /* [out] */ Int64* result)
{
    assert(b != NULL);
    Buffer* buf = (Buffer*)b;
    if (buf == NULL || buf->mEffectiveDirectAddress == 0)
    {
        return 0L;
    }
    return buf->mEffectiveDirectAddress + (buf->mPosition << buf->mElementSizeShift);
}

ECode NIOAccess::GetBaseArray(
    /* [in] */ IBuffer* b,
    /* [out] */ IInterface** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return b.hasArray() ? b.array() : null;

#endif
}

ECode NIOAccess::GetRemainingBytes(
    /* [in] */ IBuffer* b,
    /* [out] */ Int32* remaining)
{
    assert(b);
    Buffer* buf = (Buffer*)b;
    if (buf != NULL) {
        return (buf->mLimit - buf->mPosition) << buf->mElementSizeShift;
    }
    else {
        return -1;
    }
}

ECode NIOAccess::GetBaseArrayOffset(
    /* [in] */ IBuffer* b,
    /* [out] */ Int32* result)
{
    assert(b);
    Boolean hasArray;
    b->HasArray(&hasArray);
    Buffer* buf = (Buffer*)b;
    if (hasArray && buf != NULL) {
        Int32 offset, position;
        b->GetArrayOffset(&offset);
        b->GetPosition(&position);
        return (offset + position)<< buf->mElementSizeShift;
    }
    else {
        return 0;
    }
}

} // namespace IO
} // namespace Elastos
