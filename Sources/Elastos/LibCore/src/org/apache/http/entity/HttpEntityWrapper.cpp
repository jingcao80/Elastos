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

#include "org/apache/http/entity/HttpEntityWrapper.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

CAR_INTERFACE_IMPL(HttpEntityWrapper, Object, IHttpEntity)

ECode HttpEntityWrapper::IsRepeatable(
    /* [out] */ Boolean* isRepeatable)
{
    VALIDATE_NOT_NULL(isRepeatable)
    return mWrappedEntity->IsRepeatable(isRepeatable);
}

ECode HttpEntityWrapper::IsChunked(
    /* [out] */ Boolean* isChunked)
{
    VALIDATE_NOT_NULL(isChunked)
    return mWrappedEntity->IsChunked(isChunked);
}

ECode HttpEntityWrapper::GetContentLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length)
    return mWrappedEntity->GetContentLength(length);
}

ECode HttpEntityWrapper::GetContentType(
    /* [out] */ IHeader** type)
{
    VALIDATE_NOT_NULL(type)
    return mWrappedEntity->GetContentType(type);
}

ECode HttpEntityWrapper::GetContentEncoding(
    /* [out] */ IHeader** encoding)
{
    VALIDATE_NOT_NULL(encoding)
    return mWrappedEntity->GetContentEncoding(encoding);
}

ECode HttpEntityWrapper::GetContent(
    /* [out] */ IInputStream** inputStream)
{
    VALIDATE_NOT_NULL(inputStream)
    return mWrappedEntity->GetContent(inputStream);
}

ECode HttpEntityWrapper::WriteTo(
    /* [in] */ IOutputStream* outstream)
{
    return mWrappedEntity->WriteTo(outstream);
}

ECode HttpEntityWrapper::IsStreaming(
    /* [out] */ Boolean* isStreaming)
{
    VALIDATE_NOT_NULL(isStreaming)
    return mWrappedEntity->IsStreaming(isStreaming);
}

ECode HttpEntityWrapper::ConsumeContent()
{
    return mWrappedEntity->ConsumeContent();
}

ECode HttpEntityWrapper::Init(
    /* [in] */ IHttpEntity* wrapped)
{
    if (wrapped == NULL) {
        Logger::E("HttpEntityWrapper", "wrapped entity must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mWrappedEntity = wrapped;
    return NOERROR;
}

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org