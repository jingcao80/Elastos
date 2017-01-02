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

#include "CDigestInputStream.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CDigestInputStream)
CAR_INTERFACE_IMPL(CDigestInputStream, FilterInputStream, IDigestInputStream)

CDigestInputStream::CDigestInputStream()
    : mIsOn(TRUE)
{}

ECode CDigestInputStream::constructor(
    /* [in] */ IInputStream* stream,
    /* [in] */ IMessageDigest* digest)
{
    FAIL_RETURN(FilterInputStream::constructor(stream))
    mDigest = digest;
    return NOERROR;
}

ECode CDigestInputStream::GetMessageDigest(
    /* [out] */ IMessageDigest** digest)
{
    VALIDATE_NOT_NULL(digest)
    *digest = mDigest;
    REFCOUNT_ADD(*digest)
    return NOERROR;
}

ECode CDigestInputStream::SetMessageDigest(
    /* [in] */ IMessageDigest* digest)
{
    mDigest = digest;
    return NOERROR;
}

ECode CDigestInputStream::Read(
    /* [out] */ Int32* byteRead)
{
    VALIDATE_NOT_NULL(byteRead)

    // read the next byte
    FAIL_RETURN(mIn->Read(byteRead))
    // update digest only if
    // - digest functionality is on
    // - eos has not been reached
    if (mIsOn && (*byteRead != -1)) {
        mDigest->Update((Byte)*byteRead);
    }
    // return byte read
    return NOERROR;
}

ECode CDigestInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* bytesRead)
{
    VALIDATE_NOT_NULL(bytesRead)

    FAIL_RETURN(mIn->Read(buffer, byteOffset, byteCount, bytesRead))
    // update digest only if
    // - digest functionality is on
    // - eos has not been reached
    if (mIsOn && (*bytesRead != -1)) {
        mDigest->Update(buffer, byteOffset, *bytesRead);
    }
    // return number of bytes read
    return NOERROR;
}

ECode CDigestInputStream::On(
    /* [in] */ Boolean on)
{
    mIsOn = on;
    return NOERROR;
}

ECode CDigestInputStream::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    FilterInputStream::ToString(str);
    StringBuilder sb(*str);
    sb += ", ";
    sb += TO_STR(mDigest);
    sb += (mIsOn ? ", is on" : ", is off");
    return sb.ToString(str);
}

} // namespace Security
} // namespace Elastos

