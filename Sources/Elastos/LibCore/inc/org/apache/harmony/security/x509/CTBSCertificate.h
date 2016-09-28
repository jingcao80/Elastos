
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CTBSCERTIFICATE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CTBSCERTIFICATE_H__

#include "_Org_Apache_Harmony_Security_X509_CTBSCertificate.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::X501::IName;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::Math::IBigInteger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CTBSCertificate)
    , public Object
    , public ITBSCertificate
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetVersion(
        /* [out] */ Int32* pVer);

    CARAPI GetSerialNumber(
        /* [out] */ IBigInteger** ppSerialNumber);

    CARAPI GetSignature(
        /* [out] */ IAlgorithmIdentifier** ppSignature);

    CARAPI GetIssuer(
        /* [out] */ IName** ppIssuer);

    CARAPI GetValidity(
        /* [out] */ IValidity** ppValidity);

    CARAPI GetSubject(
        /* [out] */ IName** ppSubject);

    CARAPI GetSubjectPublicKeyInfo(
        /* [out] */ ISubjectPublicKeyInfo** ppInfo);

    CARAPI GetIssuerUniqueID(
        /* [out, callee] */ ArrayOf<Boolean>** ppUniqueId);

    CARAPI GetSubjectUniqueID(
        /* [out, callee] */ ArrayOf<Boolean>** ppUniqueId);

    CARAPI GetExtensions(
        /* [out] */ IExtensions** ppExtensions);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb);

    CARAPI constructor(
        /* [in] */ Int32 ver,
        /* [in] */ IBigInteger* pSerialNumber,
        /* [in] */ IAlgorithmIdentifier* pSignature,
        /* [in] */ IName* pIssuer,
        /* [in] */ IValidity* pValidity,
        /* [in] */ IName* pSubject,
        /* [in] */ ISubjectPublicKeyInfo* pSubjectPublicKeyInfo,
        /* [in] */ ArrayOf<Boolean>* pIssuerUniqueID,
        /* [in] */ ArrayOf<Boolean>* pSubjectUniqueID,
        /* [in] */ IExtensions* pExtensions);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CTBSCERTIFICATE_H__
