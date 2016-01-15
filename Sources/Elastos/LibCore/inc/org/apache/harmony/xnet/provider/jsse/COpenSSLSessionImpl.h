
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLSESSIONIMPL_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLSESSIONIMPL_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_COpenSSLSessionImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(COpenSSLSessionImpl)
{
public:
    CARAPI GetApplicationBufferSize(
        /* [out] */ Int32 * pSize);

    CARAPI GetCipherSuite(
        /* [out] */ String * pSuite);

    CARAPI GetCreationTime(
        /* [out] */ Int64 * pTime);

    CARAPI GetId(
        /* [out, callee] */ ArrayOf<Byte> ** ppId);

    CARAPI GetLastAccessedTime(
        /* [out] */ Int64 * pTime);

    CARAPI GetLocalCertificates(
        /* [out, callee] */ ArrayOf<Elastos::Security::Cert::ICertificate *> ** ppCerts);

    CARAPI GetLocalPrincipal(
        /* [out] */ Elastos::Security::IPrincipal ** ppPrincipal);

    CARAPI GetPacketBufferSize(
        /* [out] */ Int32 * pSize);

    CARAPI GetPeerCertificateChain(
        /* [out, callee] */ ArrayOf<Elastosx::Security::Cert::IX509Certificate *> ** ppChain);

    CARAPI GetPeerCertificates(
        /* [out, callee] */ ArrayOf<Elastos::Security::Cert::ICertificate *> ** ppCerts);

    CARAPI GetPeerHost(
        /* [out] */ String * pHost);

    CARAPI GetPeerPort(
        /* [out] */ Int32 * pPort);

    CARAPI GetPeerPrincipal(
        /* [out] */ Elastos::Security::IPrincipal ** ppPrincipal);

    CARAPI GetProtocol(
        /* [out] */ String * pProtocol);

    CARAPI GetSessionContext(
        /* [out] */ Elastosx::Net::Ssl::ISSLSessionContext ** ppContext);

    CARAPI GetValue(
        /* [in] */ const String& name,
        /* [out] */ IInterface ** ppValue);

    CARAPI GetValueNames(
        /* [out, callee] */ ArrayOf<String> ** ppNames);

    CARAPI Invalidate();

    CARAPI IsValid(
        /* [out] */ Boolean * pIsValid);

    CARAPI PutValue(
        /* [in] */ const String& name,
        /* [in] */ IInterface * pValue);

    CARAPI RemoveValue(
        /* [in] */ const String& name);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLSESSIONIMPL_H__
