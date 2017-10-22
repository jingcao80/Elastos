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


#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CProxy.h"
#include "elastos/droid/net/Proxy.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CProxy, Singleton, IProxy)

CAR_SINGLETON_IMPL(CProxy)

ECode CProxy::GetProxy(
    /* [in] */ IContext* ctx,
    /* [in] */ const String& url,
    /* [out] */ Elastos::Net::IProxy** result)
{
    return Proxy::GetProxy(ctx, url, result);
}

ECode CProxy::GetHost(
    /* [in] */ IContext* ctx,
    /* [out] */ String* result)
{
    return Proxy::GetHost(ctx, result);
}

ECode CProxy::GetPort(
    /* [in] */ IContext* ctx,
    /* [out] */ Int32* result)
{
    return Proxy::GetPort(ctx, result);
}

ECode CProxy::GetDefaultHost(
    /* [out] */ String* result)
{
    return Proxy::GetDefaultHost(result);
}

ECode CProxy::GetDefaultPort(
    /* [out] */ Int32* result)
{
    return Proxy::GetDefaultPort(result);
}

ECode CProxy::GetPreferredHttpHost(
    /* [in] */ IContext* context,
    /* [in] */ const String& url,
    /* [out] */ IHttpHost** result)
{
    return Proxy::GetPreferredHttpHost(context, url, result);
}

ECode CProxy::Validate(
    /* [in] */ const String& hostname,
    /* [in] */ const String& port,
    /* [in] */ const String& exclList,
    /* [out] */ Int32* result)
{
    return Proxy::Validate(hostname, port, exclList, result);
}

ECode CProxy::SetHttpProxySystemProperty(
    /* [in] */ IProxyInfo* p)
{
    return Proxy::SetHttpProxySystemProperty(p);
}

ECode CProxy::SetHttpProxySystemProperty(
    /* [in] */ const String& host,
    /* [in] */ const String& port,
    /* [in] */ const String& exclList,
    /* [in] */ IUri* pacFileUrl)
{
    return Proxy::SetHttpProxySystemProperty(host, port, exclList, pacFileUrl);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos

