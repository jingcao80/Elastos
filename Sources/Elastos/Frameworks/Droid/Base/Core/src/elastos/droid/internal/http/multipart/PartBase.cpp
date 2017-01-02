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

#include "elastos/droid/internal/http/multipart/PartBase.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

CAR_INTERFACE_IMPL(PartBase, Object, IPartBase)

ECode PartBase::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& contentType,
    /* [in] */ const String& charSet,
    /* [in] */ const String& transferEncoding)
{
    if (name.IsNull()) {
        Logger::E("PartBase", "Name must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mName = name;
    mContentType = contentType;
    mCharSet = charSet;
    mTransferEncoding = transferEncoding;

    return NOERROR;
}

ECode PartBase::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode PartBase::GetContentType(
    /* [out] */ String* contentType)
{
    VALIDATE_NOT_NULL(contentType);
    *contentType = mContentType;
    return NOERROR;
}

ECode PartBase::GetCharSet(
    /* [out] */ String* charSet)
{
    VALIDATE_NOT_NULL(charSet);
    *charSet = mCharSet;
    return NOERROR;
}

ECode PartBase::GetTransferEncoding(
    /* [out] */ String* transferEncoding)
{
    VALIDATE_NOT_NULL(transferEncoding);
    *transferEncoding = mTransferEncoding;
    return NOERROR;
}

ECode PartBase::SetCharSet(
    /* [in] */ const String& charSet)
{
    mCharSet = charSet;
    return NOERROR;
}

ECode PartBase::SetContentType(
    /* [in] */ const String& contentType)
{
    mContentType = contentType;
    return NOERROR;
}

ECode PartBase::SetName(
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
        Logger::E("PartBase", "Name must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mName = name;
    return NOERROR;
}

ECode PartBase::SetTransferEncoding(
    /* [in] */ const String& transferEncoding)
{
    mTransferEncoding = transferEncoding;
    return NOERROR;
}

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos
