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

#include "org/apache/http/entity/CEntityTemplate.h"
#include "elastos/io/CByteArrayInputStream.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

CAR_OBJECT_IMPL(CEntityTemplate)

ECode CEntityTemplate::GetContentLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length)
    *length = -1;
    return NOERROR;
}

ECode CEntityTemplate::GetContent(
    /* [out] */ IInputStream** inputStream)
{
    VALIDATE_NOT_NULL(inputStream)
    Logger::E("CEntityTemplate", "Entity template does not implement getContent()");
    return  E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CEntityTemplate::IsRepeatable(
    /* [out] */ Boolean* isRepeatable)
{
    VALIDATE_NOT_NULL(isRepeatable)
    *isRepeatable = TRUE;
    return NOERROR;
}

ECode CEntityTemplate::WriteTo(
    /* [in] */ IOutputStream* outstream)
{
    if (outstream == NULL) {
        Logger::E("CEntityTemplate", "Output stream may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return mContentproducer->WriteTo(outstream);
}

ECode CEntityTemplate::IsStreaming(
    /* [out] */ Boolean* isStreaming)
{
    VALIDATE_NOT_NULL(isStreaming)
    *isStreaming = TRUE;
    return NOERROR;
}

ECode CEntityTemplate::ConsumeContent()
{
    return NOERROR;
}

ECode CEntityTemplate::constructor(
    /* [in] */ IContentProducer* contentproducer)
{
    if (contentproducer == NULL) {
        Logger::E("CEntityTemplate", "Content producer may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mContentproducer = contentproducer;
    return NOERROR;
}

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org
