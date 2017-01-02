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

#include "elastos/droid/internal/utility/BitwiseOutputStream.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(BitwiseOutputStream, Object, IBitwiseOutputStream)

ECode BitwiseOutputStream::constructor(
    /* [in] */ Int32 startingLength)
{
    mBuf = ArrayOf<Byte>::Alloc(startingLength);
    mEnd = startingLength << 3;
    mPos = 0;
    return NOERROR;
}

ECode BitwiseOutputStream::ToByteArray(
    /* [out, callee] */ ArrayOf<Byte>** buf)
{
    VALIDATE_NOT_NULL(buf)
    Int32 len = ((UInt32)mPos >> 3) + ((mPos & 0x07) > 0 ? 1 : 0);  // &7==%8
    AutoPtr<ArrayOf<Byte> > newBuf = ArrayOf<Byte>::Alloc(len);
    newBuf->Copy(mBuf, len);
    *buf = newBuf;
    REFCOUNT_ADD(*buf)
    return NOERROR;
}

void BitwiseOutputStream::PossExpand(
    /* [in] */ Int32 bits)
{
    if ((mPos + bits) < mEnd)
        return;
    AutoPtr<ArrayOf<Byte> > newBuf = ArrayOf<Byte>::Alloc((UInt32)(mPos + bits) >> 2);
    newBuf->Copy(mBuf, (UInt32)mEnd >> 3);
    mBuf = newBuf;
    mEnd = newBuf->GetLength() << 3;
}

ECode BitwiseOutputStream::Write(
    /* [in] */ Int32 bits,
    /* [in] */ Int32 data)
{
    if ((bits < 0) || (bits > 8)) {
        Logger::E("BitwiseOutputStream", "illegal Write (bits %d)", bits);
        return E_ACCESS_EXCEPTION;
    }
    PossExpand(bits);
    data &= (UInt32)-1 >> (32 - bits);// unsigned shift
    Int32 index = (UInt32)mPos >> 3;// unsigned shift
    Int32 offset = 16 - (mPos & 0x07) - bits;  // &7==%8
    data <<= offset;
    mPos += bits;
    (*mBuf)[index] |= (UInt32)data >> 8;// unsigned shift
    if (offset < 8)
        (*mBuf)[index + 1] |= data & 0xFF;
    return NOERROR;
}

ECode BitwiseOutputStream::WriteByteArray(
    /* [in] */ Int32 bits,
    /* [in] */ ArrayOf<Byte>* arr)
{
    for (Int32 i = 0; i < arr->GetLength(); i++) {
        Int32 increment = Elastos::Core::Math::Min(8, bits - (i << 3));
        if (increment > 0) {
            FAIL_RETURN(Write(increment, (Byte)((UInt32)(*arr)[i] >> (8 - increment))));
        }
    }
    return NOERROR;
}

ECode BitwiseOutputStream::Skip(
    /* [in] */ Int32 bits)
{
    PossExpand(bits);
    mPos += bits;
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
