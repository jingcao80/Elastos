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

#include <DataBuffer.h>

namespace Elastos {
namespace RPC {

DataBuffer::DataBuffer(
    /* [in] */ Int32 cap)
    : mSize(0)
    , mCapacity(cap)
{
    mBuf = malloc(mCapacity);
}

DataBuffer::~DataBuffer()
{
    free(mBuf);
    mCapacity = mSize = 0;
}

void DataBuffer::Append(
    /* [in] */ const void* buf,
    /* [in] */ Int32 size)
{
    if (mSize + size > mCapacity) {
        Int32 newSize = ((mSize + size) * 3) / 2;
        void* data = realloc(mBuf, newSize);
        assert(data != NULL);
        mBuf = data;
        mCapacity = newSize;
    }
    memcpy((UInt8*)mBuf + mSize, buf, size);
    mSize += size;
}

void* DataBuffer::Data()
{
    return mBuf;
}

Int32 DataBuffer::DataSize()
{
    return mSize;
}

void DataBuffer::Reset()
{
    mSize = 0;
}

} // namespace RPC
} // namespace Elastos
