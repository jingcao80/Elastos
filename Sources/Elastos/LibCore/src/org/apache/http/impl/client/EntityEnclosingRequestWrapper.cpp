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

#include "Elastos.CoreLibrary.Net.h"
#include "org/apache/http/impl/client/EntityEnclosingRequestWrapper.h"

using Org::Apache::Http::IHeader;
using Org::Apache::Http::Protocol::IHTTP;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

EntityEnclosingRequestWrapper::EntityEnclosingRequestWrapper(
    /* [in] */ IHttpEntityEnclosingRequest* request)
    : RequestWrapper(IHttpRequest::Probe(request))
{
    request->GetEntity((IHttpEntity**)&mEntity);
}

CAR_INTERFACE_IMPL(EntityEnclosingRequestWrapper, RequestWrapper, IHttpEntityEnclosingRequest)

ECode EntityEnclosingRequestWrapper::SetEntity(
    /* [in] */ IHttpEntity* entity)
{
    mEntity = entity;
    return NOERROR;
}

ECode EntityEnclosingRequestWrapper::GetEntity(
    /* [out] */ IHttpEntity** entity)
{
    VALIDATE_NOT_NULL(entity)
    *entity = mEntity;
    REFCOUNT_ADD(*entity)
    return NOERROR;
}

ECode EntityEnclosingRequestWrapper::ExpectContinue(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    AutoPtr<IHeader> expect;
    GetFirstHeader(IHTTP::EXPECT_DIRECTIVE, (IHeader**)&expect);
    if (expect != NULL) {
        String value;
        expect->GetValue(&value);
        *result = IHTTP::EXPECT_CONTINUE.EqualsIgnoreCase(value);
    }
    return NOERROR;
}

Boolean EntityEnclosingRequestWrapper::IsRepeatable()
{
    if (mEntity == NULL) {
        return TRUE;
    }
    else {
        Boolean isRepeatable;
        mEntity->IsRepeatable(&isRepeatable);
        return isRepeatable;
    }
}

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org