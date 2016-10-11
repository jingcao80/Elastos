
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CALTERNATIVENAME_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CALTERNATIVENAME_H__

#include "_Org_Apache_Harmony_Security_X509_CAlternativeName.h"
#include "org/apache/harmony/security/x509/ExtensionValue.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CAlternativeName)
    , public ExtensionValue
    , public IAlternativeName
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ Boolean which,
        /* [in] */ ArrayOf<Byte>* pEncoding);

private:
    /** indicating which alternative name is presented by this object */
    Boolean mWhich;

    /** the alternative names */
    AutoPtr<IGeneralNames> mAlternativeNames;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CALTERNATIVENAME_H__
