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

#include "CCharBufferHelper.h"
#include "BufferFactory.h"
#include "CharBuffer.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(CCharBufferHelper, Singleton, ICharBufferHelper)

CAR_SINGLETON_IMPL(CCharBufferHelper)

ECode CCharBufferHelper::Allocate(
    /* [in] */ Int32 capacity,
    /* [out] */ ICharBuffer** charBuf)
{
    return CharBuffer::Allocate(capacity, charBuf);
}

ECode CCharBufferHelper::Wrap(
    /* [in] */ ArrayOf<Char32>* array,
    /* [out] */ ICharBuffer** charBuf)
{
    return CharBuffer::Wrap(array, charBuf);
}

ECode CCharBufferHelper::Wrap(
    /* [in] */ ArrayOf<Char32>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 charCount,
    /* [out] */ ICharBuffer** charBuf)
{
    return CharBuffer::Wrap(array, start, charCount, charBuf);
}

ECode CCharBufferHelper::Wrap(
    /* [in] */ ICharSequence* charSequence,
    /* [out] */ ICharBuffer**  charBuf)
{
    return CharBuffer::Wrap(charSequence, charBuf);
}

ECode CCharBufferHelper::Wrap(
    /* [in] */ ICharSequence* cs,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharBuffer** charBuf)
{
    return CharBuffer::Wrap(cs, start, end, charBuf);
}

} // namespace IO
} // namespace Elastos
