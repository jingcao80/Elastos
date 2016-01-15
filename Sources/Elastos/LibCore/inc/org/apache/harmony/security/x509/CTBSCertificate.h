
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CTBSCERTIFICATE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CTBSCERTIFICATE_H__

#include "_Org_Apache_Harmony_Security_X509_CTBSCertificate.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CTBSCertificate)
{
public:
    CARAPI GetVersion(
        /* [out] */ Int32 * pVer);

    CARAPI GetSerialNumber(
        /* [out] */ Elastos::Math::IBigInteger ** ppSerialNumber);

    CARAPI GetSignature(
        /* [out] */ Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier ** ppSignature);

    CARAPI GetIssuer(
        /* [out] */ Org::Apache::Harmony::Security::X501::IName ** ppIssuer);

    CARAPI GetValidity(
        /* [out] */ Org::Apache::Harmony::Security::X509::IValidity ** ppValidity);

    CARAPI GetSubject(
        /* [out] */ Org::Apache::Harmony::Security::X501::IName ** ppSubject);

    CARAPI GetSubjectPublicKeyInfo(
        /* [out] */ Org::Apache::Harmony::Security::X509::ISubjectPublicKeyInfo ** ppInfo);

    CARAPI GetIssuerUniqueID(
        /* [out, callee] */ ArrayOf<Boolean> ** ppUniqueId);

    CARAPI GetSubjectUniqueID(
        /* [out, callee] */ ArrayOf<Boolean> ** ppUniqueId);

    CARAPI GetExtensions(
        /* [out] */ Org::Apache::Harmony::Security::X509::IExtensions ** ppExtensions);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncoded);

    CARAPI DumpValue(
        /* [in] */ Elastos::Core::IStringBuilder * pSb);

    CARAPI constructor(
        /* [in] */ Int32 ver,
        /* [in] */ Elastos::Math::IBigInteger * pSerialNumber,
        /* [in] */ Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier * pSignature,
        /* [in] */ Org::Apache::Harmony::Security::X501::IName * pIssuer,
        /* [in] */ Org::Apache::Harmony::Security::X509::IValidity * pValidity,
        /* [in] */ Org::Apache::Harmony::Security::X501::IName * pSubject,
        /* [in] */ Org::Apache::Harmony::Security::X509::ISubjectPublicKeyInfo * pSubjectPublicKeyInfo,
        /* [in] */ ArrayOf<Boolean> * pIssuerUniqueID,
        /* [in] */ ArrayOf<Boolean> * pSubjectUniqueID,
        /* [in] */ Org::Apache::Harmony::Security::X509::IExtensions * pExtensions);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CTBSCERTIFICATE_H__
