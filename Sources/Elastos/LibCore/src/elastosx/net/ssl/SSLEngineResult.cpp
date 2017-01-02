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

#include "SSLEngineResult.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(SSLEngineResult, Object, ISSLEngineResult)

ECode SSLEngineResult::constructor(
    /* [in] */ SSLEngineResult_Status status,
    /* [in] */ SSLEngineResult_HandshakeStatus handshakeStatus,
    /* [in] */ Int32 bytesConsumed,
    /* [in] */ Int32 bytesProduced)
{
    // if (status == null) {
    //     throw new IllegalArgumentException("status == null");
    // }
    // if (handshakeStatus == null) {
    //     throw new IllegalArgumentException("handshakeStatus == null");
    // }
    if (bytesConsumed < 0) {
        //throw new IllegalArgumentException("bytesConsumed < 0: " + bytesConsumed);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (bytesProduced < 0) {
        //throw new IllegalArgumentException("bytesProduced < 0: " + bytesProduced);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mStatus = status;
    mHandshakeStatus = handshakeStatus;
    mBytesConsumed = bytesConsumed;
    mBytesProduced = bytesProduced;
    return NOERROR;
}

ECode SSLEngineResult::GetStatus(
    /* [out] */ SSLEngineResult_Status* status)
{
    VALIDATE_NOT_NULL(status)

    *status = mStatus;
    return NOERROR;
}

ECode SSLEngineResult::GetHandshakeStatus(
    /* [out] */ SSLEngineResult_HandshakeStatus* status)
{
    VALIDATE_NOT_NULL(status)

    *status = mHandshakeStatus;
    return NOERROR;
}

ECode SSLEngineResult::BytesConsumed(
    /* [out] */ Int32* bytesConsumed)
{
    VALIDATE_NOT_NULL(bytesConsumed)

    *bytesConsumed = mBytesConsumed;
    return NOERROR;
}

ECode SSLEngineResult::BytesProduced(
    /* [out] */ Int32* bytesProduced)
{
    VALIDATE_NOT_NULL(bytesProduced)

    *bytesProduced = mBytesProduced;
    return NOERROR;
}

ECode SSLEngineResult::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder sb;
    sb += "SSLEngineReport: Status = ";
    sb += mStatus;
    sb += "  HandshakeStatus = ";
    sb += mHandshakeStatus;
    sb += "\n                 bytesConsumed = ";
    sb += mBytesConsumed;
    sb += " bytesProduced = ";
    sb += mBytesProduced;

    *str = sb.ToString();
    return NOERROR;
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
