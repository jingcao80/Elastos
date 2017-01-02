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
#include "elastos/droid/net/CSSLCertificateSocketFactory.h"
#include "elastos/droid/net/CSSLCertificateSocketFactoryHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CSSLCertificateSocketFactoryHelper, Singleton, ISSLCertificateSocketFactoryHelper)

CAR_SINGLETON_IMPL(CSSLCertificateSocketFactoryHelper)

ECode CSSLCertificateSocketFactoryHelper::GetDefault(
    /* [in] */ Int32 handshakeTimeoutMillis,
    /* [out] */ ISocketFactory** result)
{
    return CSSLCertificateSocketFactory::GetDefault(handshakeTimeoutMillis, result);
}

ECode CSSLCertificateSocketFactoryHelper::GetDefault(
    /* [in] */ Int32 handshakeTimeoutMillis,
    /* [in] */ ISSLSessionCache* cache,
    /* [out] */ ISSLSocketFactory** result)
{
    return CSSLCertificateSocketFactory::GetDefault(handshakeTimeoutMillis, cache, result);
}

ECode CSSLCertificateSocketFactoryHelper::GetInsecure(
    /* [in] */ Int32 handshakeTimeoutMillis,
    /* [in] */ ISSLSessionCache* cache,
    /* [out] */ ISSLSocketFactory** result)
{
    return CSSLCertificateSocketFactory::GetInsecure(handshakeTimeoutMillis, cache, result);
}

ECode CSSLCertificateSocketFactoryHelper::GetHttpSocketFactory(
    /* [in] */ Int32 handshakeTimeoutMillis,
    /* [in] */ ISSLSessionCache* cache,
    /* [out] */ Org::Apache::Http::Conn::SSL::ISSLSocketFactory** result)
{
    return CSSLCertificateSocketFactory::GetHttpSocketFactory(handshakeTimeoutMillis, cache, result);
}

ECode CSSLCertificateSocketFactoryHelper::VerifyHostname(
    /* [in] */ ISocket* socket,
    /* [in] */ const String& hostname)
{
    return CSSLCertificateSocketFactory::VerifyHostname(socket, hostname);
}

} //namespace Net
} //namespace Droid
} //namespace Elastos
