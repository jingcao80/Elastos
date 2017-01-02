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

#include "CCheckedOutputStream.h"

namespace Elastos {
namespace Utility {
namespace Zip {

CAR_INTERFACE_IMPL(CCheckedOutputStream, FilterOutputStream, ICheckedOutputStream)

CAR_OBJECT_IMPL(CCheckedOutputStream)

ECode CCheckedOutputStream::constructor(
    /* [in] */ IOutputStream* os,
    /* [in] */ IChecksum* cs)
{
    FAIL_RETURN(FilterOutputStream::constructor(os));
    mCheck = cs;
    return NOERROR;
}

ECode CCheckedOutputStream::GetChecksum(
    /* [out] */ IChecksum** checksum)
{
    VALIDATE_NOT_NULL(checksum);
    *checksum = mCheck;
    REFCOUNT_ADD(*checksum);
    return NOERROR;
}

ECode CCheckedOutputStream::Write(
    /* [in] */ Int32 val)
{
    FAIL_RETURN(mOut->Write(val));
    return mCheck->Update(val);
}

ECode CCheckedOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 off,
    /* [in] */ Int32 nbytes)
{
    FAIL_RETURN(mOut->Write(buf, off, nbytes));
    return mCheck->Update(buf, off, nbytes);
}


} // namespace Zip
} // namespace Utility
} // namespace Elastos
