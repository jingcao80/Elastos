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

#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/cookie/params/CCookieSpecParamBean.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Cookie {
namespace Params {

CAR_INTERFACE_IMPL(CCookieSpecParamBean, Object, ICookieSpecParamBean)

CAR_OBJECT_IMPL(CCookieSpecParamBean)

ECode CCookieSpecParamBean::SetDatePatterns(
    /* [in] */ ICollection* patterns)
{
    AutoPtr<IHttpParams> params;
    return mParams->SetParameter(ICookieSpecPNames::DATE_PATTERNS, patterns, (IHttpParams**)&params);
}

ECode CCookieSpecParamBean::SetSingleHeader(
    /* [in] */ Boolean singleHeader)
{
    AutoPtr<IHttpParams> params;
    return mParams->SetBooleanParameter(ICookieSpecPNames::SINGLE_COOKIE_HEADER, singleHeader, (IHttpParams**)&params);
}

ECode CCookieSpecParamBean::constructor(
    /* [in] */ IHttpParams* params)
{
    return Init(params);
}

} // namespace Params
} // namespace Cookie
} // namespace Http
} // namespace Apache
} // namespace Org
