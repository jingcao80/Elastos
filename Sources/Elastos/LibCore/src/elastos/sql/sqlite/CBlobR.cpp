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

#include "CBlobR.h"

namespace Elastos {
namespace Sql {
namespace SQLite {

CAR_OBJECT_IMPL(CBlobR)

CAR_INTERFACE_IMPL(CBlobR, InputStream, IBlobR)

ECode CBlobR::constructor(
    /* [in] */ Elastos::Sql::SQLite::IBlob* blob)
{
    mBlob = (CBlob *)blob;
    mPos = 0;
    return NOERROR;
}

ECode CBlobR::Available(
    /* [out] */ Int32* number)
{
    Int32 ret = mBlob->mSize - mPos;
    *number = (ret < 0) ? 0 : ret;
    return NOERROR;
}

ECode CBlobR::Mark(
    /* [in] */ Int32 readLimit)
{
    return NOERROR;
}

ECode CBlobR::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    AutoPtr<ArrayOf<Byte> > b = ArrayOf<Byte>::Alloc(1);
    Int32 n;
    FAIL_RETURN(mBlob->Read(b, 0, mPos, b->GetLength(), &n))
    if (n > 0) {
        mPos += n;
        *value = (*b)[0];
    }
    *value = -1;
    return NOERROR;
}

ECode CBlobR::ReadBytes(
    /* [in] */ ArrayOf<Byte> * buffer,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;
    VALIDATE_NOT_NULL(buffer)

    Int32 n;
    FAIL_RETURN(mBlob->Read(buffer, 0, mPos, buffer->GetLength(), &n))
    if (n > 0) {
        mPos += n;
        *number = n;
    }
    *number = -1;
    return NOERROR;
}

ECode CBlobR::ReadBytes(
    /* [in] */ ArrayOf<Byte> * b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;
    VALIDATE_NOT_NULL(b)

    if (off + len > b->GetLength()) {
        len = b->GetLength() - off;
    }
    if (len < 0) {
        *number = -1;
    }
    if (len == 0) {
        *number = 0;
    }
    Int32 n;
    FAIL_RETURN(mBlob->Read(b, off, mPos, len, &n))
    if (n > 0) {
        mPos += n;
        *number = n;
    }
    *number = -1;
    return NOERROR;
}

ECode CBlobR::Reset()
{
    return NOERROR;
}

ECode CBlobR::Close()
{
    mBlob->Close();
    mBlob = NULL;
    mPos = 0;
    return NOERROR;
}

ECode CBlobR::Skip(
    /* [in] */ Int64 n,
    /* [out] */ Int64* number)
{
    Int64 ret = mPos + n;
    if (ret < 0) {
        ret = 0;
        mPos = 0;
    } else if (ret > mBlob->mSize) {
        ret = mBlob->mSize;
        mPos = mBlob->mSize;
    } else {
        mPos = (Int32) ret;
    }
    *number = ret;
    return NOERROR;
}

} // namespace SQLite
} // namespace Sql
} // namespace Elastos
