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

#include "elastos/droid/database/CharArrayBuffer.h"

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL(CharArrayBuffer, Object, ICharArrayBuffer)

CharArrayBuffer::CharArrayBuffer()
    : mSizeCopied(0)
{}

ECode CharArrayBuffer::constructor(
    /* [in] */ Int32 size)
{
    mData = ArrayOf<Char32>::Alloc(size);
    return NOERROR;
}

ECode CharArrayBuffer::constructor(
    /* [in] */ ArrayOf<Char32>* buf)
{
    mData = buf;
    return NOERROR;
}

ECode CharArrayBuffer::GetData(
    /* [out, callee] */ ArrayOf<Char32>** data)
{
    VALIDATE_NOT_NULL(data)
    *data = mData;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

ECode CharArrayBuffer::SetData(
    /* [in] */ ArrayOf<Char32>* data)
{
    mData = data;
    return NOERROR;
}

ECode CharArrayBuffer::GetSizeCopied(
    /* [out] */ Int32* sizeCopied)
{
    VALIDATE_NOT_NULL(sizeCopied)
    *sizeCopied = mSizeCopied;
    return NOERROR;
}

ECode CharArrayBuffer::SetSizeCopied(
    /* [in] */ Int32 sizeCopied)
{
    mSizeCopied = sizeCopied;
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
