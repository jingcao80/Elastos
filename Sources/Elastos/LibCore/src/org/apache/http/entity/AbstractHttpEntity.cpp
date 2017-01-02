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

#include "org/apache/http/entity/AbstractHttpEntity.h"
#include "org/apache/http/message/CBasicHeader.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Protocol::IHTTP;
using Org::Apache::Http::Message::CBasicHeader;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

AbstractHttpEntity::AbstractHttpEntity()
    : mChunked(FALSE)
{}

CAR_INTERFACE_IMPL_2(AbstractHttpEntity, Object, IAbstractHttpEntity, IHttpEntity)

ECode AbstractHttpEntity::GetContentType(
    /* [out] */ IHeader** type)
{
    VALIDATE_NOT_NULL(type)
    *type = mContentType;
    REFCOUNT_ADD(*type)
    return NOERROR;
}

ECode AbstractHttpEntity::GetContentEncoding(
    /* [out] */ IHeader** encoding)
{
    VALIDATE_NOT_NULL(encoding)
    *encoding = mContentEncoding;
    REFCOUNT_ADD(*encoding)
    return NOERROR;
}

ECode AbstractHttpEntity::IsChunked(
    /* [out] */ Boolean* isChunked)
{
    VALIDATE_NOT_NULL(isChunked)
    *isChunked = mChunked;
    return NOERROR;
}

ECode AbstractHttpEntity::SetContentType(
    /* [in] */ IHeader* contentType)
{
    mContentType = contentType;
    return NOERROR;
}

ECode AbstractHttpEntity::SetContentType(
    /* [in] */ const String& ctString)
{
    AutoPtr<IHeader> h;
    if (!ctString.IsNull()) {
        CBasicHeader::New(IHTTP::CONTENT_TYPE, ctString, (IHeader**)&h);
    }
    return SetContentType(h);
}

ECode AbstractHttpEntity::SetContentEncoding(
    /* [in] */ IHeader* contentEncoding)
{
    mContentEncoding = contentEncoding;
    return NOERROR;
}

ECode AbstractHttpEntity::SetContentEncoding(
    /* [in] */ const String& ceString)
{
    AutoPtr<IHeader> h;
    if (!ceString.IsNull()) {
        CBasicHeader::New(IHTTP::CONTENT_ENCODING, ceString, (IHeader**)&h);
    }
    return SetContentEncoding(h);
}

ECode AbstractHttpEntity::SetChunked(
    /* [in] */ Boolean b)
{
    mChunked = b;
    return NOERROR;
}

ECode AbstractHttpEntity::ConsumeContent()
{
    Boolean result;
    if (IsStreaming(&result), result) {
        Logger::E("AbstractHttpEntity", "streaming entity does not implement consumeContent()");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org