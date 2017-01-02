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

#include "org/apache/http/params/CHttpProtocolParamBean.h"
#include "org/apache/http/params/HttpProtocolParams.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

CAR_INTERFACE_IMPL(CHttpProtocolParamBean, Object, IHttpProtocolParamBean)

CAR_OBJECT_IMPL(CHttpProtocolParamBean)

ECode CHttpProtocolParamBean::SetHttpElementCharset(
    /* [in] */ const String& httpElementCharset)
{
    return HttpProtocolParams::SetHttpElementCharset(mParams, httpElementCharset);
}

ECode CHttpProtocolParamBean::SetContentCharset(
    /* [in] */ const String& contentCharset)
{
    return HttpProtocolParams::SetContentCharset(mParams, contentCharset);
}

ECode CHttpProtocolParamBean::SetVersion(
    /* [in] */ IHttpVersion* version)
{
    return HttpProtocolParams::SetVersion(mParams, IProtocolVersion::Probe(version));
}

ECode CHttpProtocolParamBean::SetUserAgent(
    /* [in] */ const String& userAgent)
{
    return HttpProtocolParams::SetUserAgent(mParams, userAgent);
}

ECode CHttpProtocolParamBean::SetUseExpectContinue(
    /* [in] */ Boolean useExpectContinue)
{
    return HttpProtocolParams::SetUseExpectContinue(mParams, useExpectContinue);
}

ECode CHttpProtocolParamBean::constructor(
    /* [in] */ IHttpParams* params)
{
    return Init(params);
}

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org
