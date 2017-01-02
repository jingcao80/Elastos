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

#include "elastos/droid/internal/utility/BitwiseInputStream.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(BitwiseInputStream, Object, IBitwiseInputStream)

ECode BitwiseInputStream::constructor(
    /* [in] */ ArrayOf<Byte>* buf)
{
    mBuf = buf;
    mEnd = buf->GetLength() << 3;
    mPos = 0;
    return NOERROR;
}

ECode BitwiseInputStream::Available(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    *num = mEnd - mPos;
    return NOERROR;
}

ECode BitwiseInputStream::Read(
    /* [in] */ Int32 bits,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    Int32 index = (UInt32)mPos >> 3;// unsigned shift
    Int32 offset = 16 - (mPos & 0x07) - bits;  // &7==%8
    if ((bits < 0) || (bits > 8) || ((mPos + bits) > mEnd)) {
        Logger::E("BitwiseInputStream", "illegal read (pos %d, end %d, bits %d)",
                mPos, mEnd, bits);
        return E_ACCESS_EXCEPTION;
    }
    Int32 data = ((*mBuf)[index] & 0xFF) << 8;
    if (offset < 8)
        data |= (*mBuf)[index + 1] & 0xFF;
    data = (UInt32)data >> offset;// unsigned shift
    data &= (UInt32)-1 >> (32 - bits);// unsigned shift;
    mPos += bits;
    *num = data;
    return NOERROR;
}

ECode BitwiseInputStream::ReadByteArray(
    /* [in] */ Int32 bits,
    /* [out] */ ArrayOf<Byte>** buf)
{
    Int32 bytes = ((UInt32)bits >> 3) + ((bits & 0x07) > 0 ? 1 : 0);  // &7==%8
    AutoPtr<ArrayOf<Byte> > arr = ArrayOf<Byte>::Alloc(bytes);
    for (Int32 i = 0; i < bytes; i++) {
        Int32 increment = Elastos::Core::Math::Min(8, bits - (i << 3));
        Int32 num;
        FAIL_RETURN(Read(increment, &num));
        (*arr)[i] = (Byte)(num << (8 - increment));
    }
    *buf = arr;
    REFCOUNT_ADD(*buf)
    return NOERROR;
}

ECode BitwiseInputStream::Skip(
    /* [in] */ Int32 bits)
{
    if ((mPos + bits) > mEnd) {
        Logger::E("BitwiseInputStream", "illegal skip (pos %d, end %d, bits %d)",
                mPos, mEnd, bits);
        return E_ACCESS_EXCEPTION;
    }
    mPos += bits;
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
