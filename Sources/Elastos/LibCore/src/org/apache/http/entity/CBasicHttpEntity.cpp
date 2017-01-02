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

#include "org/apache/http/entity/CBasicHttpEntity.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

CBasicHttpEntity::CBasicHttpEntity()
    : mContentObtained(FALSE)
    , mLength(0)
{}

CAR_INTERFACE_IMPL(CBasicHttpEntity, AbstractHttpEntity, IBasicHttpEntity)

CAR_OBJECT_IMPL(CBasicHttpEntity)

ECode CBasicHttpEntity::GetContentLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length)
    *length = mLength;
    return NOERROR;
}

ECode CBasicHttpEntity::GetContent(
    /* [out] */ IInputStream** inputStream)
{
    VALIDATE_NOT_NULL(inputStream)
    *inputStream = NULL;
    if (mContent == NULL) {
        Logger::E("CBasicHttpEntity", "Content has not been provided");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mContentObtained) {
        Logger::E("CBasicHttpEntity", "Content has been consumed");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mContentObtained = TRUE;
    *inputStream = mContent;
    REFCOUNT_ADD(*inputStream)
    return NOERROR;
}

ECode CBasicHttpEntity::IsRepeatable(
    /* [out] */ Boolean* isRepeatable)
{
    VALIDATE_NOT_NULL(isRepeatable)
    *isRepeatable = FALSE;
    return NOERROR;
}

ECode CBasicHttpEntity::SetContentLength(
    /* [in] */ Int64 len)
{
    mLength= len;
    return NOERROR;
}

ECode CBasicHttpEntity::SetContent(
    /* [in] */ IInputStream* instream)
{
    mContent = instream;
    mContentObtained = FALSE;
    return NOERROR;
}

ECode CBasicHttpEntity::WriteTo(
    /* [in] */ IOutputStream* outstream)
{
    if (outstream == NULL) {
        Logger::E("CBasicHttpEntity", "Output stream may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInputStream> instream;
    GetContent((IInputStream**)&instream);
    Int32 l;
    AutoPtr< ArrayOf<Byte> > tmp = ArrayOf<Byte>::Alloc(2048);
    while (instream->Read(tmp, &l), l != -1) {
        outstream->Write(tmp, 0, l);
    }
    return NOERROR;
}

ECode CBasicHttpEntity::IsStreaming(
    /* [out] */ Boolean* isStreaming)
{
    VALIDATE_NOT_NULL(isStreaming)
    *isStreaming = !mContentObtained && mContent != NULL;
    return NOERROR;
}

ECode CBasicHttpEntity::ConsumeContent()
{
    if (mContent != NULL) {
        mContent->Close(); // reads to the end of the entity
    }
    return NOERROR;
}

ECode CBasicHttpEntity::constructor()
{
    mLength = -1;
    return NOERROR;
}

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org
