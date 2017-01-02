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

#include "org/apache/http/impl/io/IdentityInputStream.h"
#include "org/apache/http/impl/io/AbstractMessageParser.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/core/Math.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::Math;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Protocol::IHTTP;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

IdentityInputStream::IdentityInputStream(
    /* [in] */ ISessionInputBuffer* in)
    : InputStream()
    , mClosed(FALSE)
{
    if (in == NULL) {
        Logger::E("IdentityInputStream", "Session input buffer may not be null");
        assert(0);
        // throw new IllegalArgumentException("Session input buffer may not be null");
    }
    mIn = in;
}

ECode IdentityInputStream::Available(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    Boolean available;
    if (!mClosed && (mIn->IsDataAvailable(10, &available), available)) {
        *number = 1;
    }
    else {
        *number = 0;
    }
    return NOERROR;
}

ECode IdentityInputStream::Close()
{
    mClosed = TRUE;
    return NOERROR;
}

ECode IdentityInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    if (mClosed) {
        *value = -1;
        return NOERROR;
    }
    else {
        return mIn->Read(value);
    }
}

ECode IdentityInputStream::Read(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    if (mClosed) {
        *number = -1;
        return NOERROR;
    }
    else {
        return mIn->Read(b, off, len, number);
    }
}

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org