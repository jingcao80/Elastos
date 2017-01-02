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

#include "org/apache/http/entity/CBufferedHttpEntity.h"
#include "org/apache/http/utility/EntityUtils.h"
#include "elastos/io/CByteArrayInputStream.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Utility::EntityUtils;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

CAR_OBJECT_IMPL(CBufferedHttpEntity)

ECode CBufferedHttpEntity::GetContentLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length)
    if (mBuffer != NULL) {
        *length = mBuffer->GetLength();
        return NOERROR;
    }
    else {
        return mWrappedEntity->GetContentLength(length);
    }
}

ECode CBufferedHttpEntity::GetContent(
    /* [out] */ IInputStream** inputStream)
{
    VALIDATE_NOT_NULL(inputStream)
    if (mBuffer != NULL) {
        AutoPtr<IByteArrayInputStream> stream;
        CByteArrayInputStream::New(mBuffer, (IByteArrayInputStream**)&stream);
        *inputStream = IInputStream::Probe(stream);
        REFCOUNT_ADD(*inputStream)
        return NOERROR;
    }
    else {
        return mWrappedEntity->GetContent(inputStream);
    }
}

ECode CBufferedHttpEntity::IsChunked(
    /* [out] */ Boolean* isChunked)
{
    VALIDATE_NOT_NULL(isChunked)
    if (mBuffer == NULL) {
        Boolean result;
        mWrappedEntity->IsChunked(&result);
        *isChunked = result;
    }
    else {
        *isChunked = FALSE;
    }
    return NOERROR;
}

ECode CBufferedHttpEntity::IsRepeatable(
    /* [out] */ Boolean* isRepeatable)
{
    VALIDATE_NOT_NULL(isRepeatable)
    *isRepeatable = TRUE;
    return NOERROR;
}

ECode CBufferedHttpEntity::WriteTo(
    /* [in] */ IOutputStream* outstream)
{
    if (outstream == NULL) {
        Logger::E("CBufferedHttpEntity", "Output stream may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mBuffer != NULL) {
        return outstream->Write(mBuffer);
    }
    else {
        return mWrappedEntity->WriteTo(outstream);
    }
}

ECode CBufferedHttpEntity::IsStreaming(
    /* [out] */ Boolean* isStreaming)
{
    VALIDATE_NOT_NULL(isStreaming)
    if (mBuffer == NULL) {
        Boolean result;
        mWrappedEntity->IsStreaming(&result);
        *isStreaming = result;
    }
    else {
        *isStreaming = FALSE;
    }
    return NOERROR;
}

ECode CBufferedHttpEntity::constructor(
    /* [in] */ IHttpEntity* entity)
{
    FAIL_RETURN(Init(entity))
    Boolean isRepeatable;
    Int64 len;
    if ((entity->IsRepeatable(&isRepeatable), !isRepeatable)
            || (entity->GetContentLength(&len), len < 0)) {
        EntityUtils::ToByteArray(entity, (ArrayOf<Byte>**)&mBuffer);
    }
    else {
        mBuffer = NULL;
    }
    return NOERROR;
}

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org
