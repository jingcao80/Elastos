
#ifndef __ELASTOS_SECURITY_CERT_X509CRL_H__
#define __ELASTOS_SECURITY_CERT_X509CRL_H__

#include "CRL.h"

using Elastos::Security::IPublicKey;
using Elastosx::Security::Auth::X500::IX500Principal;
using Elastos::Utility::IDate;
using Elastos::Math::IBigInteger;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Security {
namespace Cert {

extern "C" const InterfaceID EIID_X509CRL;

class X509CRL
    : public CRL
    , public IX509Extension
{
public:
    CAR_INTERFACE_DECL()

    virtual CARAPI Equals(
        /* [in] */ IInterface *other,
        /* [out] */ Boolean *result);

    virtual CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    virtual CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> **encoded) = 0;

    virtual CARAPI Verify(
        /* [in] */ IPublicKey *key) = 0;

    virtual CARAPI VerifyEx(
        /* [in] */ IPublicKey *key,
        /* [in] */ const String& sigProvider) = 0;

    virtual CARAPI GetVersion(
        /* [out] */ Int32 *vs) = 0;

    virtual CARAPI GetIssuerDN(
        /* [out] */ IPrincipal **issuer) = 0;

    virtual CARAPI GetIssuerX500Principal(
        /* [out] */ IX500Principal **issuer);

    virtual CARAPI GetThisUpdate(
        /* [out] */ IDate **thisUpdate) = 0;

    virtual CARAPI GetNextUpdate(
        /* [out] */ IDate **nextUpdate) = 0;

    virtual CARAPI GetRevokedCertificate(
        /* [in] */ IBigInteger *serialNumber,
        /* [out] */ IX509CRLEntry **crlEntry) = 0;

    virtual CARAPI GetRevokedCertificateEx(
        /* [in] */ IX509Certificate *certificate,
        /* [out] */ IX509CRLEntry **crlEntry);

    virtual CARAPI GetRevokedCertificates(
        /* [out] */ ISet **ctrlEntrys) = 0;

    virtual CARAPI GetCriticalExtensionOIDs(
        /* [out] */ ISet **oids) = 0;

    virtual CARAPI GetExtensionValue(
        /* [in] */ const String& oid,
        /* [out, callee] */ ArrayOf<Byte> **extensionValue) = 0;

    virtual CARAPI GetNonCriticalExtensionOIDs(
        /* [out] */ ISet **oids) = 0;

    virtual CARAPI HasUnsupportedCriticalExtension(
        /* [out] */ Boolean *result) = 0;
protected:
    X509CRL();
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_X509CRL_H__
