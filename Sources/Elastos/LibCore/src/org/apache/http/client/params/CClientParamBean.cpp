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
#include "org/apache/http/client/params/CClientParamBean.h"
#include "elastos/core/CString.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Params {

CAR_INTERFACE_IMPL(CClientParamBean, HttpAbstractParamBean, IClientParamBean)

CAR_OBJECT_IMPL(CClientParamBean)

ECode CClientParamBean::SetConnectionManagerFactoryClassName(
    /* [in] */ const String& factory)
{
    AutoPtr<ICharSequence> cs;
    CString::New(factory, (ICharSequence**)&cs);
    AutoPtr<IHttpParams> params;
    return mParams->SetParameter(IClientPNames::CONNECTION_MANAGER_FACTORY_CLASS_NAME, cs,
            (IHttpParams**)&params);
}

ECode CClientParamBean::SetConnectionManagerFactory(
    /* [in] */ IClientConnectionManagerFactory* factory)
{
    AutoPtr<IHttpParams> params;
    return mParams->SetParameter(IClientPNames::CONNECTION_MANAGER_FACTORY, factory,
            (IHttpParams**)&params);
}

ECode CClientParamBean::SetHandleRedirects(
    /* [in] */ Boolean handle)
{
    AutoPtr<IHttpParams> params;
    return mParams->SetBooleanParameter(IClientPNames::HANDLE_REDIRECTS, handle,
            (IHttpParams**)&params);
}

ECode CClientParamBean::SetRejectRelativeRedirect(
    /* [in] */ Boolean reject)
{
    AutoPtr<IHttpParams> params;
    return mParams->SetBooleanParameter(IClientPNames::REJECT_RELATIVE_REDIRECT, reject,
            (IHttpParams**)&params);
}

ECode CClientParamBean::SetMaxRedirects(
    /* [in] */ Int32 maxRedirects)
{
    AutoPtr<IHttpParams> params;
    return mParams->SetInt32Parameter(IClientPNames::MAX_REDIRECTS, maxRedirects,
            (IHttpParams**)&params);
}

ECode CClientParamBean::SetAllowCircularRedirects(
    /* [in] */ Boolean allow)
{
    AutoPtr<IHttpParams> params;
    return mParams->SetBooleanParameter(IClientPNames::ALLOW_CIRCULAR_REDIRECTS, allow,
            (IHttpParams**)&params);
}

ECode CClientParamBean::SetHandleAuthentication(
    /* [in] */ Boolean handle)
{
    AutoPtr<IHttpParams> params;
    return mParams->SetBooleanParameter(IClientPNames::HANDLE_AUTHENTICATION, handle,
            (IHttpParams**)&params);
}

ECode CClientParamBean::SetCookiePolicy(
    /* [in] */ const String& policy)
{
    AutoPtr<ICharSequence> cs;
    CString::New(policy, (ICharSequence**)&cs);
    AutoPtr<IHttpParams> params;
    return mParams->SetParameter(IClientPNames::COOKIE_POLICY, cs,
            (IHttpParams**)&params);
}

ECode CClientParamBean::SetVirtualHost(
    /* [in] */ IHttpHost* host)
{
    AutoPtr<IHttpParams> params;
    return mParams->SetParameter(IClientPNames::VIRTUAL_HOST, host,
            (IHttpParams**)&params);
}

ECode CClientParamBean::SetDefaultHeaders(
    /* [in] */ ICollection* headers)
{
    AutoPtr<IHttpParams> params;
    return mParams->SetParameter(IClientPNames::DEFAULT_HEADERS, headers,
            (IHttpParams**)&params);
}

ECode CClientParamBean::SetDefaultHost(
    /* [in] */ IHttpHost* host)
{
    AutoPtr<IHttpParams> params;
    return mParams->SetParameter(IClientPNames::DEFAULT_HOST, host,
            (IHttpParams**)&params);
}

ECode CClientParamBean::constructor(
    /* [in] */ IHttpParams* params)
{
    return Init(params);
}

} // namespace Params
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org