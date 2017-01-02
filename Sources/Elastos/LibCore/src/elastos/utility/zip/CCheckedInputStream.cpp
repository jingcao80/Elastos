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

#include "CCheckedInputStream.h"
#include "Math.h"
#include "CStreams.h"

using Elastos::Core::Math;
using Elastos::IO::EIID_IInputStream;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;

namespace Elastos {
namespace Utility {
namespace Zip {

CAR_INTERFACE_IMPL(CCheckedInputStream, FilterInputStream, ICheckedInputStream)

CAR_OBJECT_IMPL(CCheckedInputStream)

ECode CCheckedInputStream::constructor(
    /* [in] */ IInputStream* is,
    /* [in] */ IChecksum* csum)
{
    FAIL_RETURN(FilterInputStream::constructor(is))
    mCheck = csum;
    return NOERROR;
}

ECode CCheckedInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    FAIL_RETURN(mIn->Read(value));
    if (*value != -1) {
        mCheck->Update(*value);
    }
    return NOERROR;
}

ECode CCheckedInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 off,
    /* [in] */ Int32 nbytes,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;
    VALIDATE_NOT_NULL(buffer);

    FAIL_RETURN(mIn->Read(buffer, off, nbytes, number));
    if (*number != -1) {
        mCheck->Update(buffer, off, *number);
    }
    return NOERROR;
}

ECode CCheckedInputStream::GetChecksum(
    /* [out] */ IChecksum** checksum)
{
    VALIDATE_NOT_NULL(checksum);

    *checksum = mCheck;
    REFCOUNT_ADD(*checksum);
    return NOERROR;
}

ECode CCheckedInputStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number);
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    return streams->SkipByReading(this, byteCount, number);
}

ECode CCheckedInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;
    VALIDATE_NOT_NULL(buffer);
    return Read(buffer, 0, buffer->GetLength(), number);
}


} // namespace Zip
} // namespace Utility
} // namespace Elastos
