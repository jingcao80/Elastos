
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENSION_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENSION_H__

#include "_Org_Apache_Harmony_Security_X509_CExtension.h"
//#include "Elastos.CoreLibrary.Security.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::IO::IOutputStream;
using Elastos::Security::Cert::IExtension;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CExtension)
    , public Object
    , public IX509Extension
    , public IExtension
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetId(
        /* [out] */ String* id);

    CARAPI IsCritical(
        /* [out] */ Boolean* isCritical);

    CARAPI GetValue(
        /* [out, callee] */ ArrayOf<Byte>** value);

    CARAPI Encode(
        /* [in] */ IOutputStream* outValue);

    CARAPI GetExtnID(
        /* [out] */ String* pExtnID);

    CARAPI GetCritical(
        /* [out] */ Boolean* pCritical);

    CARAPI GetExtnValue(
        /* [out, callee] */ ArrayOf<Byte>** ppExtnValue);

    CARAPI GetRawExtnValue(
        /* [out, callee] */ ArrayOf<Byte>** ppRawExtnValue);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI Equals(
        /* [in] */ IInterface* pExt,
        /* [out] */ Boolean* pResult);

    CARAPI GetHashCode(
        /* [out] */ Int32* pHashCode);

    CARAPI GetDecodedExtensionValue(
        /* [out] */ IExtensionValue** ppExtensionValue);

    CARAPI GetKeyUsageValue(
        /* [out] */ IKeyUsage** ppUsageValue);

    CARAPI GetBasicConstraintsValue(
        /* [out] */ IBasicConstraints** ppBasicConstrainsValue);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ const String& extnID,
        /* [in] */ Boolean critical,
        /* [in] */ IExtensionValue* pExtnValueObject);

    CARAPI constructor(
        /* [in] */ const String& extnID,
        /* [in] */ Boolean critical,
        /* [in] */ ArrayOf<Byte>* pExtnValue);

    CARAPI constructor(
        /* [in] */ ArrayOf<Int32>* pExtnID,
        /* [in] */ Boolean critical,
        /* [in] */ ArrayOf<Byte>* pExtnValue2);

    CARAPI constructor(
        /* [in] */ const String& extnID,
        /* [in] */ ArrayOf<Byte>* pExtnValue);

    CARAPI constructor(
        /* [in] */ ArrayOf<Int32>* pExtnID,
        /* [in] */ ArrayOf<Byte>* pExtnValue2);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENSION_H__
