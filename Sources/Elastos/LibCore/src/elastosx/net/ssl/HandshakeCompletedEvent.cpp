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

#include "HandshakeCompletedEvent.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(HandshakeCompletedEvent, EventObject, IHandshakeCompletedEvent)

ECode HandshakeCompletedEvent::constructor(
    /* [in] */ ISSLSocket* sock,
    /* [in] */ ISSLSession* s)
{
    FAIL_RETURN(EventObject::constructor(IObject::Probe(sock)))
    mSession = s;
    return NOERROR;
}

ECode HandshakeCompletedEvent::GetSession(
    /* [out] */ ISSLSession** session)
{
    VALIDATE_NOT_NULL(session)

    *session = mSession;
    REFCOUNT_ADD(*session)
    return NOERROR;
}

ECode HandshakeCompletedEvent::GetCipherSuite(
    /* [out] */ String* suite)
{
    VALIDATE_NOT_NULL(suite)

    return mSession->GetCipherSuite(suite);
}

ECode HandshakeCompletedEvent::GetLocalCertificates(
    /* [out, callee] */ ArrayOf<ICertificate*>** cert)
{
    VALIDATE_NOT_NULL(cert)

    return mSession->GetLocalCertificates(cert);
}

ECode HandshakeCompletedEvent::GetPeerCertificates(
    /* [out, callee] */ ArrayOf<ICertificate*>** cert)
{
    VALIDATE_NOT_NULL(cert)

    return mSession->GetPeerCertificates(cert);
}

ECode HandshakeCompletedEvent::GetPeerCertificateChain(
    /* [out, callee] */ ArrayOf<IX509Certificate*>** chain)
{
    VALIDATE_NOT_NULL(chain)

    return mSession->GetPeerCertificateChain(chain);
}

ECode HandshakeCompletedEvent::GetPeerPrincipal(
    /* [out] */ IPrincipal** principal)
{
    VALIDATE_NOT_NULL(principal)

    return mSession->GetPeerPrincipal(principal);
}

ECode HandshakeCompletedEvent::GetLocalPrincipal(
    /* [out] */ IPrincipal** principal)
{
    VALIDATE_NOT_NULL(principal)

    return mSession->GetLocalPrincipal(principal);
}

ECode HandshakeCompletedEvent::GetSocket(
    /* [out] */ ISSLSocket** socket)
{
    VALIDATE_NOT_NULL(socket)

    *socket = ISSLSocket::Probe(mSource);
    REFCOUNT_ADD(*socket)
    return NOERROR;

}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
