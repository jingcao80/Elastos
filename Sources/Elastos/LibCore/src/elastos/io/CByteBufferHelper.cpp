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

#include "CByteBufferHelper.h"
#include "ByteBuffer.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(CByteBufferHelper, Singleton, IByteBufferHelper)

CAR_SINGLETON_IMPL(CByteBufferHelper)

ECode CByteBufferHelper::Allocate(
    /* [in] */ Int32 capacity,
    /* [out] */ IByteBuffer** byteBuf)
{
    return ByteBuffer::Allocate(capacity, byteBuf);
}

ECode CByteBufferHelper::AllocateDirect(
    /* [in] */ Int32 capacity,
    /* [out] */ IByteBuffer** byteBuf)
{
    return ByteBuffer::AllocateDirect(capacity, byteBuf);
}

ECode CByteBufferHelper::Wrap(
    /* [in] */ ArrayOf<Byte>* array,
    /* [out] */ IByteBuffer** byteBuf)
{
    return ByteBuffer::Wrap(array, byteBuf);
}

ECode CByteBufferHelper::Wrap(
    /* [in] */ ArrayOf<Byte>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 byteCount,
    /* [out] */ IByteBuffer** byteBuf)
{
    return ByteBuffer::Wrap(array, start, byteCount, byteBuf);
}

} // namespace IO
} // namespace Elastos
