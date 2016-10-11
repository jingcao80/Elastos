
#ifndef __ORG_CONSCRYPT_OPENSSLSESSIONIMPL_H__
#define __ORG_CONSCRYPT_OPENSSLSESSIONIMPL_H__

#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastosx::Net::Ssl::ISSLSession;
using Elastosx::Net::Ssl::ISSLSessionContext;
using Elastos::Security::IPrincipal;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::Cert::ICertificate;
using Elastos::Utility::IMap;

namespace Org {
namespace Conscrypt {

/**
 * Implementation of the class OpenSSLSessionImpl
 * based on OpenSSL.
 */
class OpenSSLSessionImpl
    : public Object
    , public ISSLSession
    , public IOpenSSLSessionImpl
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLSessionImpl();

    /**
     * Class constructor creates an SSL session context given the appropriate
     * SSL parameters.
     */
    CARAPI constructor(
        /* [in] */ Int64 sslSessionNativePointer,
        /* [in] */ ArrayOf<IX509Certificate*>* localCertificates,
        /* [in] */ ArrayOf<IX509Certificate*>* peerCertificates,
        /* [in] */ String peerHost,
        /* [in] */ Int32 peerPort,
        /* [in] */ IAbstractSessionContext* sessionContext);

    /**
     * Constructs a session from a byte[] containing DER data. This
     * allows loading the saved session.
     * @throws IOException
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* derData,
        /* [in] */ String peerHost,
        /* [in] */ Int32 peerPort,
        /* [in] */ ArrayOf<IX509Certificate*>* peerCertificates,
        /* [in] */ IAbstractSessionContext* sessionContext);

    /**
     * Gets the identifier of the actual SSL session
     * @return array of sessions' identifiers.
     */
    CARAPI GetId(
        /* [out] */ ArrayOf<Byte>** result);

    /**
     * Reset the id field to the current value found in the native
     * SSL_SESSION. It can change during the lifetime of the session
     * because while a session is created during initial handshake,
     * with handshake_cutthrough, the SSL_do_handshake may return
     * before we have read the session ticket from the server side and
     * therefore have computed no id based on the SHA of the ticket.
     */
    CARAPI ResetId();

    /**
     * Get the session object in DER format. This allows saving the session
     * data or sharing it with other processes.
     */
    CARAPI GetEncoded(
        /* [out] */ ArrayOf<Byte>** result);

    /**
     * Gets the creation time of the SSL session.
     * @return the session's creation time in milliseconds since the epoch
     */
    CARAPI GetCreationTime(
        /* [out] */ Int64* result);

    /**
     * Returns the last time this concrete SSL session was accessed. Accessing
     * here is to mean that a new connection with the same SSL context data was
     * established.
     *
     * @return the session's last access time in milliseconds since the epoch
     */
    CARAPI GetLastAccessedTime(
        /* [out] */ Int64* result);

    /**
     * Returns the largest buffer size for the application's data bound to this
     * concrete SSL session.
     * @return the largest buffer size
     */
    CARAPI GetApplicationBufferSize(
        /* [out] */ Int32* result);

    /**
     * Returns the largest SSL/TLS packet size one can expect for this concrete
     * SSL session.
     * @return the largest packet size
     */
    CARAPI GetPacketBufferSize(
        /* [out] */ Int32* result);

    /**
     * Returns the principal (subject) of this concrete SSL session used in the
     * handshaking phase of the connection.
     * @return a X509 certificate or null if no principal was defined
     */
    CARAPI GetLocalPrincipal(
        /* [out] */ IPrincipal** result);

    /**
     * Returns the certificate(s) of the principal (subject) of this concrete SSL
     * session used in the handshaking phase of the connection. The OpenSSL
     * native method supports only RSA certificates.
     * @return an array of certificates (the local one first and then eventually
     *         that of the certification authority) or null if no certificate
     *         were used during the handshaking phase.
     */
    CARAPI GetLocalCertificates(
        /* [out] */ ArrayOf<ICertificate*>** result);

    /**
     * Returns the certificate(s) of the peer in this SSL session
     * used in the handshaking phase of the connection.
     * Please notice hat this method is superseded by
     * <code>getPeerCertificates()</code>.
     * @return an array of X509 certificates (the peer's one first and then
     *         eventually that of the certification authority) or null if no
     *         certificate were used during the SSL connection.
     * @throws SSLPeerUnverifiedException if either a non-X.509 certificate
     *         was used (i.e. Kerberos certificates) or the peer could not
     *         be verified.
     */
    CARAPI GetPeerCertificateChain(
        /* [out] */ ArrayOf<Elastosx::Security::Cert::IX509Certificate*>** result);

    /**
     * Return the identity of the peer in this SSL session
     * determined via certificate(s).
     * @return an array of X509 certificates (the peer's one first and then
     *         eventually that of the certification authority) or null if no
     *         certificate were used during the SSL connection.
     * @throws SSLPeerUnverifiedException if either a non-X.509 certificate
     *         was used (i.e. Kerberos certificates) or the peer could not
     *         be verified.
     */
    CARAPI GetPeerCertificates(
        /* [out] */ ArrayOf<ICertificate*>** result);

    /**
     * The identity of the principal that was used by the peer during the SSL
     * handshake phase is returned by this method.
     * @return a X500Principal of the last certificate for X509-based
     *         cipher suites.
     * @throws SSLPeerUnverifiedException if either a non-X.509 certificate
     *         was used (i.e. Kerberos certificates) or the peer does not exist.
     *
     */
    CARAPI GetPeerPrincipal(
        /* [out] */ IPrincipal** result);

    /**
     * The peer's host name used in this SSL session is returned. It is the host
     * name of the client for the server; and that of the server for the client.
     * It is not a reliable way to get a fully qualified host name: it is mainly
     * used internally to implement links for a temporary cache of SSL sessions.
     *
     * @return the host name of the peer, or null if no information is
     *         available.
     *
     */
    CARAPI GetPeerHost(
        /* [out] */ String* result);

    /**
     * Returns the peer's port number for the actual SSL session. It is the port
     * number of the client for the server; and that of the server for the
     * client. It is not a reliable way to get a peer's port number: it is
     * mainly used internally to implement links for a temporary cache of SSL
     * sessions.
     * @return the peer's port number, or -1 if no one is available.
     *
     */
    CARAPI GetPeerPort(
        /* [out] */ Int32* result);

    /**
     * Returns a string identifier of the crypto tools used in the actual SSL
     * session. For example AES_256_WITH_MD5.
     */
    CARAPI GetCipherSuite(
        /* [out] */ String* result);

    /**
     * Returns the standard version name of the SSL protocol used in all
     * connections pertaining to this SSL session.
     */
    CARAPI GetProtocol(
        /* [out] */ String* result);

    /**
     * Returns the context to which the actual SSL session is bound. A SSL
     * context consists of (1) a possible delegate, (2) a provider and (3) a
     * protocol.
     * @return the SSL context used for this session, or null if it is
     * unavailable.
     */
    CARAPI GetSessionContext(
        /* [out] */ ISSLSessionContext** result);

    /**
     * Returns a boolean flag signaling whether a SSL session is valid
     * and available for resuming or joining or not.
     *
     * @return true if this session may be resumed.
     */
    CARAPI IsValid(
        /* [out] */ Boolean* result);

    /**
     * It invalidates a SSL session forbidding any resumption.
     */
    CARAPI Invalidate();

    /**
     * Returns the object which is bound to the the input parameter name.
     * This name is a sort of link to the data of the SSL session's application
     * layer, if any exists.
     *
     * @param name the name of the binding to find.
     * @return the value bound to that name, or null if the binding does not
     *         exist.
     * @throws IllegalArgumentException if the argument is null.
     */
    CARAPI GetValue(
        /* [in] */ const String& name,
        /* [out] */ IInterface** result);

    /**
     * Returns an array with the names (sort of links) of all the data
     * objects of the application layer bound into the SSL session.
     *
     * @return a non-null (possibly empty) array of names of the data objects
     *         bound to this SSL session.
     */
    CARAPI GetValueNames(
        /* [out] */ ArrayOf<String>** result);

    /**
     * A link (name) with the specified value object of the SSL session's
     * application layer data is created or replaced. If the new (or existing)
     * value object implements the <code>SSLSessionBindingListener</code>
     * interface, that object will be notified in due course.
     *
     * @param name the name of the link (no null are
     *            accepted!)
     * @param value data object that shall be bound to
     *            name.
     * @throws IllegalArgumentException if one or both argument(s) is null.
     */
    CARAPI PutValue(
        /* [in] */ const String& name,
        /* [in] */ IInterface* value);

    /**
     * Removes a link (name) with the specified value object of the SSL
     * session's application layer data.
     *
     * <p>If the value object implements the <code>SSLSessionBindingListener</code>
     * interface, the object will receive a <code>valueUnbound</code> notification.
     *
     * @param name the name of the link (no null are
     *            accepted!)
     * @throws IllegalArgumentException if the argument is null.
     */
    CARAPI RemoveValue(
        /* [in] */ const String& name);

    CARAPI Finalize();

private:
    /**
     * Provide a value to initialize the volatile peerCertificateChain
     * field based on the native SSL_SESSION
     */
    AutoPtr<ArrayOf<Elastosx::Security::Cert::IX509Certificate*> > CreatePeerCertificateChain();

    /**
     * Throw SSLPeerUnverifiedException on null or empty peerCertificates array
     */
    void CheckPeerCertificatesPresent();

public:
    Int64 mLastAccessedTime;
    AutoPtr<ArrayOf<IX509Certificate*> > mLocalCertificates;
    AutoPtr<ArrayOf<IX509Certificate*> > mPeerCertificates;
    Int64 mSslSessionNativePointer;

private:
    Int64 mCreationTime;
    Boolean mIsValid;
    AutoPtr<IMap> mValues;
    /*volatile*/ AutoPtr<ArrayOf<Elastosx::Security::Cert::IX509Certificate*> > mPeerCertificateChain;
    String mPeerHost;
    Int32 mPeerPort;
    String mCipherSuite;
    String mProtocol;
    AutoPtr<IAbstractSessionContext> mSessionContext;
    AutoPtr<ArrayOf<Byte> > mId;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLSESSIONIMPL_H__