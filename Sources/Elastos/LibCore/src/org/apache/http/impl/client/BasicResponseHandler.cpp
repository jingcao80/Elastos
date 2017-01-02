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

#include "org/apache/http/impl/client/BasicResponseHandler.h"
#include "org/apache/http/utility/EntityUtils.h"
#include "elastos/core/CString.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::Client::EIID_IResponseHandler;
using Org::Apache::Http::Utility::EntityUtils;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

CAR_INTERFACE_IMPL(BasicResponseHandler, Object, IResponseHandler)

ECode BasicResponseHandler::HandleResponse(
    /* [in] */ IHttpResponse* response,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;
    AutoPtr<IStatusLine> statusLine;
    response->GetStatusLine((IStatusLine**)&statusLine);
    Int32 code;
    if (statusLine->GetStatusCode(&code), code >= 300) {
        String phrase;
        statusLine->GetReasonPhrase(&phrase);
        Logger::E("BasicResponseHandler", "%d, %s", code, phrase.string());
        return E_HTTP_RESPONSE_EXCEPTION;
    }

    AutoPtr<IHttpEntity> entity;
    response->GetEntity((IHttpEntity**)&entity);
    if (entity != NULL) {
        String str;
        EntityUtils::ToString(entity, &str);
        AutoPtr<ICharSequence> cs;
        CString::New(str, (ICharSequence**)&cs);
        *value = (IInterface*)cs.Get();
        REFCOUNT_ADD(*value)
    }
    return NOERROR;
}

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org