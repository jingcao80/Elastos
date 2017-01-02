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

#ifndef __ELASTOSX_NET_SSL_HANDSHAKECOMPLETEDEVENT_H__
#define __ELASTOSX_NET_SSL_HANDSHAKECOMPLETEDEVENT_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "EventObject.h"

using Elastos::Utility::EventObject;
using Elastos::Security::IPrincipal;
using Elastos::Security::Cert::ICertificate;
using Elastosx::Net::Ssl::ISSLSocket;
using Elastosx::Net::Ssl::ISSLSession;
using Elastosx::Security::Cert::IX509Certificate;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The event object encapsulating the information about a completed SSL
 * handshake on a SSL connection.
 */
class HandshakeCompletedEvent
    : public EventObject
    , public IHandshakeCompletedEvent
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a new {@code HandshakeCompletedEvent} with the specified SSL
     * socket and SSL session.
     *
     * @param sock
     *            the SSL socket.
     * @param s
     *            the SSL session.
     */
    CARAPI constructor(
        /* [in] */ ISSLSocket* sock,
        /* [in] */ ISSLSession* s);

    /**
     * Returns the SSL session associated with this event.
     *
     * @return the SSL session associated with this event.
     */
    CARAPI GetSession(
        /* [out] */ ISSLSession** session);

    /**
     * Returns the name of the cipher suite negotiated during this handshake.
     *
     * @return the name of the cipher suite negotiated during this handshake.
     */
    CARAPI GetCipherSuite(
        /* [out] */ String* suite);

    /**
     * Returns the list of local certificates used during the handshake. These
     * certificates were sent to the peer.
     *
     * @return Returns the list of certificates used during the handshake with
     *         the local identity certificate followed by CAs, or {@code null}
     *         if no certificates were used during the handshake.
     */
    CARAPI GetLocalCertificates(
        /* [out, callee] */ ArrayOf<ICertificate*>** cert);

    /**
     * Return the list of certificates identifying the peer during the
     * handshake.
     *
     * @return the list of certificates identifying the peer with the peer's
     *         identity certificate followed by CAs.
     * @throws SSLPeerUnverifiedException
     *             if the identity of the peer has not been verified.
     */
    CARAPI GetPeerCertificates(
        /* [out, callee] */ ArrayOf<ICertificate*>** cert);

    /**
     * Returns the list of certificates identifying the peer. The peer's
     * identity certificate is followed by the validated certificate authority
     * certificates.
     * <p>
     * <b>Replaced by:</b> {@link #getPeerCertificates()}
     *
     * @return the list of certificates identifying the peer
     * @throws SSLPeerUnverifiedException
     *             if the identity of the peer has not been verified.
     */
    CARAPI GetPeerCertificateChain(
        /* [out, callee] */ ArrayOf<IX509Certificate*>** chain);

    /**
     * Returns the {@code Principal} identifying the peer.
     *
     * @return the {@code Principal} identifying the peer.
     * @throws SSLPeerUnverifiedException
     *             if the identity of the peer has not been verified.
     */
    CARAPI GetPeerPrincipal(
        /* [out] */ IPrincipal** principal);

    /**
     * Returns the {@code Principal} used to identify during the handshake.
     *
     * @return the {@code Principal} used to identify during the handshake.
     */
    CARAPI GetLocalPrincipal(
        /* [out] */ IPrincipal** principal);

    /**
     * Returns the SSL socket that produced this event.
     *
     * @return the SSL socket that produced this event.
     */
    CARAPI GetSocket(
        /* [out] */ ISSLSocket** socket);

private:
    /*transient*/ AutoPtr<ISSLSession> mSession;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_HANDSHAKECOMPLETEDEVENT_H__
