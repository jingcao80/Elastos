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

#include "org/apache/http/message/BasicHttpEntityEnclosingRequest.h"
#include "org/apache/http/message/CBasicRequestLine.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::Protocol::IHTTP;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_INTERFACE_IMPL(BasicHttpEntityEnclosingRequest, BasicHttpRequest, IHttpEntityEnclosingRequest)

ECode BasicHttpEntityEnclosingRequest::Init(
    /* [in] */ const String& method,
    /* [in] */ const String& uri)
{
    return BasicHttpRequest::Init(method, uri);
}

ECode BasicHttpEntityEnclosingRequest::Init(
    /* [in] */ const String& method,
    /* [in] */ const String& uri,
    /* [in] */ IProtocolVersion* ver)
{
    AutoPtr<IRequestLine> line;
    CBasicRequestLine::New(method, uri, ver, (IRequestLine**)&line);
    return Init(line);
}

ECode BasicHttpEntityEnclosingRequest::Init(
    /* [in] */ IRequestLine* requestline)
{
    return BasicHttpRequest::Init(requestline);
}

ECode BasicHttpEntityEnclosingRequest::GetEntity(
    /* [out] */ IHttpEntity** entity)
{
    VALIDATE_NOT_NULL(entity)
    *entity = mEntity;
    REFCOUNT_ADD(*entity)
    return NOERROR;
}

ECode BasicHttpEntityEnclosingRequest::SetEntity(
    /* [in] */ IHttpEntity* entity)
{
    mEntity = entity;
    return NOERROR;
}

ECode BasicHttpEntityEnclosingRequest::ExpectContinue(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IHeader> expect;
    GetFirstHeader(IHTTP::EXPECT_DIRECTIVE, (IHeader**)&expect);
    String value;
    *result = expect != NULL
            && (expect->GetValue(&value), IHTTP::EXPECT_CONTINUE.EqualsIgnoreCase(value));
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
