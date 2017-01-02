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

#include "FileLock.h"

namespace Elastos {
namespace IO {
namespace Channels {

CAR_INTERFACE_IMPL_2(FileLock, Object, ICloseable, IFileLock)

FileLock::FileLock(
    /* [in] */ IFileChannel* channel,
    /* [in] */ Int64 position,
    /* [in] */ Int64 size,
    /* [in] */ Boolean shared)
{
    assert(position >= 0 && size >= 0 && position + size >= 0);

    mChannel = channel;
    mPosition = position;
    mSize = size;
    mShared = shared;
}

ECode FileLock::Channel(
    /* [out] */ IFileChannel** channel )
{
    VALIDATE_NOT_NULL(channel)
    *channel = mChannel;
    REFCOUNT_ADD(*channel);
    return NOERROR;
}

ECode FileLock::GetPosition(
    /* [out] */ Int64* position)
{
    VALIDATE_NOT_NULL(position)

    *position = mPosition;
    return NOERROR;
}

ECode FileLock::GetSize(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)

    *size = mSize;
    return NOERROR;
}

ECode FileLock::IsShared(
    /* [out] */ Boolean* shared)
{
    VALIDATE_NOT_NULL(shared)

    *shared = mShared;
    return NOERROR;
}

ECode FileLock::Overlaps(
    /* [in] */ Int64 start,
    /* [in] */ Int64 length,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    const Int64 end = mPosition + mSize - 1;
    const Int64 newEnd = start + length - 1;
    if(end < start || mPosition > newEnd)
    {
       *result = FALSE;
    }
    else
    {
       *result = TRUE;
    }
    return NOERROR;
}

ECode FileLock::Close()
{
    return ReleaseLock();
}

ECode FileLock::ToString(
    /* [in] */ String* string)
{
    VALIDATE_NOT_NULL(string)

    String str;
    str.AppendFormat("FileLock[position=%d, size=%d shared=%d]", mPosition, mSize, mShared);
    *string = str;
    return NOERROR;
}

} // namespace Channels
} // namespace IO
} // namespace Elastos
