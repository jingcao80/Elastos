
#ifndef __ELASTOS_DROID_NET_HTTP_DELEGATINGSSLSESSION_H__
#define __ELASTOS_DROID_NET_HTTP_DELEGATINGSSLSESSION_H__

#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.Security.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Security::Cert::ICertificate;
using Elastos::Security::IPrincipal;
using Elastosx::Net::Ssl::ISSLSession;
using Elastosx::Net::Ssl::ISSLSessionContext;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * This is only used when a {@code certificate} is available but usage
 * requires a {@link SSLSession}.
 *
 * @hide
 */
class DelegatingSSLSession
    : public Object
    , public ISSLSession
    , public IDelegatingSSLSession
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    // @Override
    CARAPI GetApplicationBufferSize(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetCipherSuite(
        /* [out] */ String* result);

    // @Override
    CARAPI GetCreationTime(
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetId(
        /* [out, callee] */ ArrayOf<Byte>** result);

    // @Override
    CARAPI GetLastAccessedTime(
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetLocalCertificates(
        /* [out, callee] */ ArrayOf<ICertificate*>** result);

    // @Override
    CARAPI GetLocalPrincipal(
        /* [out] */ IPrincipal** result);

    // @Override
    CARAPI GetPacketBufferSize(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetPeerCertificateChain(
        /* [out, callee] */ ArrayOf<Elastosx::Security::Cert::IX509Certificate*>** result);

    // @Override
    CARAPI GetPeerCertificates(
        /* [out, callee] */ ArrayOf<ICertificate*>** result);

    // @Override
    CARAPI GetPeerHost(
        /* [out] */ String* result);

    // @Override
    CARAPI GetPeerPort(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetPeerPrincipal(
        /* [out] */ IPrincipal** result);

    // @Override
    CARAPI GetProtocol(
        /* [out] */ String* result);

    // @Override
    CARAPI GetSessionContext(
        /* [out] */ ISSLSessionContext** result);

    // @Override
    CARAPI GetValue(
        /* [in] */ const String& name,
        /* [out] */ IInterface** result);

    // @Override
    CARAPI GetValueNames(
        /* [out, callee] */ ArrayOf<String>** result);

    // @Override
    CARAPI Invalidate();

    // @Override
    CARAPI IsValid(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI PutValue(
        /* [in] */ const String& name,
        /* [in] */ IInterface* value);

    // @Override
    CARAPI RemoveValue(
        /* [in] */ const String& name);
};

class DelegatingSSLSessionCertificateWrap
    : public DelegatingSSLSession
    , public IDelegatingSSLSessionCertificateWrap
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ICertificate* certificate);

    // @Override
    CARAPI GetPeerCertificates(
        /* [out, callee] */ ArrayOf<ICertificate*>** result);

private:
    /* const */ AutoPtr<ICertificate> mCertificate;
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_DELEGATINGSSLSESSION_H__
