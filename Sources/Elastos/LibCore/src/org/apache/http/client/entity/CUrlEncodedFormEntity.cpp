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

#include "org/apache/http/client/entity/CUrlEncodedFormEntity.h"
#include "org/apache/http/client/utils/URLEncodedUtils.h"

using Org::Apache::Http::Client::Utils::URLEncodedUtils;
using Org::Apache::Http::Protocol::IHTTP;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Entity {

CAR_OBJECT_IMPL(CUrlEncodedFormEntity)

ECode CUrlEncodedFormEntity::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<CUrlEncodedFormEntity> ver;
    CUrlEncodedFormEntity::NewByFriend((CUrlEncodedFormEntity**)&ver);
    CloneImpl((StringEntity*)ver);
    *obj = ver->Probe(EIID_IInterface);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CUrlEncodedFormEntity::constructor(
    /* [in] */ IList* parameters,
    /* [in] */ const String& encoding)
{
    FAIL_RETURN(Init(URLEncodedUtils::Format(parameters, encoding), encoding))
    return SetContentType(URLEncodedUtils::CONTENT_TYPE);
}

ECode CUrlEncodedFormEntity::constructor(
    /* [in] */ IList* parameters)
{
    FAIL_RETURN(Init(URLEncodedUtils::Format(parameters, IHTTP::DEFAULT_CONTENT_CHARSET), IHTTP::DEFAULT_CONTENT_CHARSET))
    return SetContentType(URLEncodedUtils::CONTENT_TYPE);
}

ECode CUrlEncodedFormEntity::constructor()
{
    return NOERROR;
}

} // namespace Entity
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org
