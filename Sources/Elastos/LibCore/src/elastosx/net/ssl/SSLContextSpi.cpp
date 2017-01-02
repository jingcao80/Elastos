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

#include "Elastos.CoreLibrary.Net.h"
#include "SSLContextSpi.h"
#include "CSSLParameters.h"

using Elastos::Net::ISocket;
using Elastosx::Net::Ssl::CSSLParameters;

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(SSLContextSpi, Object, ISSLContextSpi)

ECode SSLContextSpi::EngineGetDefaultSSLParameters(
    /* [out] */ ISSLParameters** param)
{
    VALIDATE_NOT_NULL(param)

    return CreateSSLParameters(FALSE, param);
}

ECode SSLContextSpi::EngineGetSupportedSSLParameters(
    /* [out] */ ISSLParameters** param)
{
    VALIDATE_NOT_NULL(param)

    return CreateSSLParameters(TRUE, param);
}

ECode SSLContextSpi::CreateSSLParameters(
    /* [in] */ Boolean supported,
    /* [out] */ ISSLParameters** param)
{
    VALIDATE_NOT_NULL(param)

    //try {
        AutoPtr<ISSLSocketFactory> factory;
        FAIL_RETURN(EngineGetSocketFactory((ISSLSocketFactory**)&factory))
        AutoPtr<ISocket> socket;
        FAIL_RETURN(ISocketFactory::Probe(factory)->CreateSocket((ISocket**)&socket))
        AutoPtr<ISSLSocket> s = ISSLSocket::Probe(socket);
        AutoPtr<ISSLParameters> p;
        CSSLParameters::New((ISSLParameters**)&p);
        AutoPtr<ArrayOf<String> > cipherSuites;
        AutoPtr<ArrayOf<String> > protocols;
        if (supported) {
            FAIL_RETURN(s->GetSupportedCipherSuites((ArrayOf<String>**)&cipherSuites))
            FAIL_RETURN(s->GetSupportedProtocols((ArrayOf<String>**)&protocols))
        } else {
            FAIL_RETURN(s->GetEnabledCipherSuites((ArrayOf<String>**)&cipherSuites))
            FAIL_RETURN(s->GetEnabledProtocols((ArrayOf<String>**)&protocols))
        }
        FAIL_RETURN(p->SetCipherSuites(cipherSuites))
        FAIL_RETURN(p->SetProtocols(protocols))
        Boolean auth;
        FAIL_RETURN(s->GetNeedClientAuth(&auth))
        FAIL_RETURN(p->SetNeedClientAuth(auth))
        FAIL_RETURN(s->GetWantClientAuth(&auth))
        FAIL_RETURN(p->SetWantClientAuth(auth))
        *param = p;
        REFCOUNT_ADD(*param);
        return NOERROR;
    //} catch (IOException e) {
        /*
         * SSLContext.getDefaultSSLParameters specifies to throw
         * UnsupportedOperationException if there is a problem getting the
         * parameters
         */
        //throw new UnsupportedOperationException("Could not access supported SSL parameters");
    //}
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
