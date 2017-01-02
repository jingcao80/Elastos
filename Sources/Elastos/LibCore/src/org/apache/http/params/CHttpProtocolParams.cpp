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

#include "org/apache/http/params/CHttpProtocolParams.h"
#include "org/apache/http/params/HttpProtocolParams.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

CAR_INTERFACE_IMPL_2(CHttpProtocolParams, Object, IHttpProtocolParams, ICoreProtocolPNames)

CAR_SINGLETON_IMPL(CHttpProtocolParams)

ECode CHttpProtocolParams::GetHttpElementCharset(
    /* [in] */ IHttpParams* params,
    /* [out] */ String* charset)
{
    VALIDATE_NOT_NULL(charset)
    return HttpProtocolParams::GetHttpElementCharset(params, charset);
}

ECode CHttpProtocolParams::SetHttpElementCharset(
    /* [in] */ IHttpParams* params,
    /* [in] */ const String& charset)
{
    return HttpProtocolParams::SetHttpElementCharset(params, charset);
}

ECode CHttpProtocolParams::GetContentCharset(
    /* [in] */ IHttpParams* params,
    /* [out] */ String* charset)
{
    VALIDATE_NOT_NULL(charset)
    return HttpProtocolParams::GetContentCharset(params, charset);
}

ECode CHttpProtocolParams::SetContentCharset(
    /* [in] */ IHttpParams* params,
    /* [in] */ const String& charset)
{
    return HttpProtocolParams::SetContentCharset(params, charset);
}

ECode CHttpProtocolParams::GetVersion(
    /* [in] */ IHttpParams* params,
    /* [out] */ IProtocolVersion** ver)
{
    VALIDATE_NOT_NULL(ver)
    return HttpProtocolParams::GetVersion(params, ver);
}

ECode CHttpProtocolParams::SetVersion(
    /* [in] */ IHttpParams* params,
    /* [in] */ IProtocolVersion* version)
{
    return HttpProtocolParams::SetVersion(params, version);
}

ECode CHttpProtocolParams::GetUserAgent(
    /* [in] */ IHttpParams* params,
    /* [out] */ String* agent)
{
    VALIDATE_NOT_NULL(agent)
    return HttpProtocolParams::GetUserAgent(params, agent);
}

ECode CHttpProtocolParams::SetUserAgent(
    /* [in] */ IHttpParams* params,
    /* [in] */ const String& useragent)
{
    return HttpProtocolParams::SetUserAgent(params, useragent);
}

ECode CHttpProtocolParams::UseExpectContinue(
    /* [in] */ IHttpParams* params,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return HttpProtocolParams::UseExpectContinue(params, result);
}

ECode CHttpProtocolParams::SetUseExpectContinue(
    /* [in] */ IHttpParams* params,
    /* [in] */ Boolean b)
{
    return HttpProtocolParams::SetUseExpectContinue(params, b);
}

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org
