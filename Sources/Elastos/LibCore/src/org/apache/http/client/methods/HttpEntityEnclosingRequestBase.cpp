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

#include "org/apache/http/client/methods/HttpEntityEnclosingRequestBase.h"
#include "org/apache/http/client/utils/CloneUtils.h"

using Org::Apache::Http::IHeader;
using Org::Apache::Http::Client::Utils::CloneUtils;
using Org::Apache::Http::Protocol::IHTTP;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Methods {

HttpEntityEnclosingRequestBase::HttpEntityEnclosingRequestBase()
    : HttpRequestBase()
{}

CAR_INTERFACE_IMPL(HttpEntityEnclosingRequestBase, HttpRequestBase, IHttpEntityEnclosingRequest)

ECode HttpEntityEnclosingRequestBase::GetEntity(
    /* [out] */ IHttpEntity** entity)
{
    VALIDATE_NOT_NULL(entity)
    *entity = mEntity;
    REFCOUNT_ADD(*entity)
    return NOERROR;
}

ECode HttpEntityEnclosingRequestBase::SetEntity(
    /* [in] */ IHttpEntity* entity)
{
    mEntity = entity;
    return NOERROR;
}

ECode HttpEntityEnclosingRequestBase::ExpectContinue(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IHeader> expect;
    GetFirstHeader(IHTTP::EXPECT_DIRECTIVE, (IHeader**)&expect);
    if (expect != NULL) {
        String value;
        expect->GetValue(&value);
        *result = IHTTP::EXPECT_CONTINUE.EqualsIgnoreCase(value);
    }
    else {
        *result = FALSE;
    }
    return NOERROR;
}

ECode HttpEntityEnclosingRequestBase::CloneImpl(
    /* [in] */ HttpEntityEnclosingRequestBase* clone)
{
    HttpRequestBase::CloneImpl(clone);
    if (mEntity != NULL) {
        AutoPtr<IObject> o;
        CloneUtils::Clone(IObject::Probe(mEntity), (IObject**)&o);
        clone->mEntity = IHttpEntity::Probe(o);
    }
    return NOERROR;
}

} // namespace Methods
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org