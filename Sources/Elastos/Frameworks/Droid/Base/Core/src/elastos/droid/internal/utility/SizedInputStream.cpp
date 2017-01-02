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

#include <Elastos.CoreLibrary.Libcore.h>
#include "elastos/droid/internal/utility/SizedInputStream.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;
using Libcore::IO::CStreams;
using Libcore::IO::IStreams;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(SizedInputStream, InputStream, ISizedInputStream)

ECode SizedInputStream::constructor(
    /* [in] */ IInputStream* wrapped,
    /* [in] */ Int64 length)
{
    mWrapped = wrapped;
    mLength = length;
    return NOERROR;
}

// @Override
ECode SizedInputStream::Close()
{
    FAIL_RETURN(InputStream::Close())
    return ICloseable::Probe(mWrapped)->Close();
}

// @Override
ECode SizedInputStream::Read(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    return streams->ReadSingleByte(this, num);
}

// @Override
ECode SizedInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    if (mLength <= 0) {
        *num = -1;
        return NOERROR;
    }
    else if (byteCount > mLength) {
        byteCount = (Int32)mLength;
    }

    Int32 n;
    mWrapped->Read(buffer, byteOffset, byteCount, &n);
    if (n == -1) {
        if (mLength > 0) {
            Logger::E("SizedInputStream", "Unexpected EOF; expected %lld more bytes", mLength);
            return E_IO_EXCEPTION;
        }
    }
    else {
        mLength -= n;
    }
    *num = n;
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
