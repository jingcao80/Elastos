
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENDEDKEYUSAGE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENDEDKEYUSAGE_H__

#include "_Org_Apache_Harmony_Security_X509_CExtendedKeyUsage.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::IList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CExtendedKeyUsage)
    , public Object
    , public IExtendedKeyUsage
    , public IExtensionValue
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb);

    CARAPI GetExtendedKeyUsage(
        /* [out] */ IList** ppExtendedKeyUsage);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pEncoding);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENDEDKEYUSAGE_H__
