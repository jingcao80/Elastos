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

#include "org/apache/http/impl/DefaultHttpResponseFactory.h"
#include "org/apache/http/impl/EnglishReasonPhraseCatalog.h"
#include "org/apache/http/message/CBasicStatusLine.h"
#include "org/apache/http/message/CBasicHttpResponse.h"
#include "elastos/utility/CLocaleHelper.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::Message::CBasicStatusLine;
using Org::Apache::Http::Message::CBasicHttpResponse;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {

DefaultHttpResponseFactory::DefaultHttpResponseFactory(
    /* [in] */ IReasonPhraseCatalog* catalog)
{
    Init(catalog);
}

DefaultHttpResponseFactory::DefaultHttpResponseFactory()
{
    Init(EnglishReasonPhraseCatalog::INSTANCE);
}

CAR_INTERFACE_IMPL(DefaultHttpResponseFactory, Object, IHttpResponseFactory)

ECode DefaultHttpResponseFactory::Init(
    /* [in] */ IReasonPhraseCatalog* catalog)
{
    if (catalog == NULL) {
        Logger::E("DefaultHttpResponseFactory", "Reason phrase catalog must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mReasonCatalog = catalog;
    return NOERROR;
}

ECode DefaultHttpResponseFactory::NewHttpResponse(
    /* [in] */ IProtocolVersion* ver,
    /* [in] */ Int32 status,
    /* [in] */ IHttpContext* context,
    /* [out] */ IHttpResponse** response)
{
    VALIDATE_NOT_NULL(response)
    *response = NULL;
    if (ver == NULL) {
        Logger::E("DefaultHttpResponseFactory", "HTTP version may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ILocale> loc;
    DetermineLocale(context, (ILocale**)&loc);
    String reason;
    mReasonCatalog->GetReason(status, loc, &reason);
    AutoPtr<IStatusLine> statusline;
    CBasicStatusLine::New(ver, status, reason, (IStatusLine**)&statusline);
    return CBasicHttpResponse::New(statusline, mReasonCatalog, loc, response);
}

ECode DefaultHttpResponseFactory::NewHttpResponse(
    /* [in] */ IStatusLine* statusline,
    /* [in] */ IHttpContext* context,
    /* [out] */ IHttpResponse** response)
{
    VALIDATE_NOT_NULL(response)
    *response = NULL;
    if (statusline == NULL) {
        Logger::E("DefaultHttpResponseFactory", "Status line may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ILocale> loc;
    DetermineLocale(context, (ILocale**)&loc);
    return CBasicHttpResponse::New(statusline, mReasonCatalog, loc, response);
}

ECode DefaultHttpResponseFactory::DetermineLocale(
    /* [in] */ IHttpContext* context,
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    return helper->GetDefault(locale);
}

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org