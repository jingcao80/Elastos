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

#include "CDigestOutputStream.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CDigestOutputStream)
CAR_INTERFACE_IMPL(CDigestOutputStream, FilterOutputStream, IDigestOutputStream)
CDigestOutputStream::CDigestOutputStream()
    : mIsOn(TRUE)
{}

ECode CDigestOutputStream::constructor(
    /* [in] */ IOutputStream* stream,
    /* [in] */ IMessageDigest* digest)
{
    FilterOutputStream::constructor(stream);
    mDigest = digest;
    return NOERROR;
}

ECode CDigestOutputStream::GetMessageDigest(
    /* [out] */ IMessageDigest** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDigest;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CDigestOutputStream::SetMessageDigest(
    /* [in] */ IMessageDigest* digest)
{
    mDigest = digest;
    return NOERROR;
}

ECode CDigestOutputStream::Write(
    /* [in] */ Int32 b) /*throws IOException*/
{
    // update digest only if digest functionality is on
    if (mIsOn) {
        mDigest->Update((Byte)b);
    }
    // write the byte
    mOut->Write(b);
    return NOERROR;
}

ECode CDigestOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len) /*throws IOException*/
{
    // update digest only if digest functionality is on
    if (mIsOn) {
        mDigest->Update(b, off, len);
    }
    // write len bytes
    mOut->Write(b, off, len);
    return NOERROR;
}

ECode CDigestOutputStream::On(
    /* [in] */ Boolean on)
{
    mIsOn = on;
    return NOERROR;
}

ECode CDigestOutputStream::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String s;
    FilterOutputStream::ToString(&s);
    StringBuilder sb(s);
    sb.Append(", ");
    mDigest->ToString(&s);
    sb.Append(s);
    sb.Append(mIsOn ? ", is on" : ", is off");
    return sb.ToString(result);
}

} // namespace Security
} // namespace Elastos

