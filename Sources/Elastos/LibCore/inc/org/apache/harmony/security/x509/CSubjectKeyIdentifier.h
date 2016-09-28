
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CSUBJECTKEYIDENTIFIER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CSUBJECTKEYIDENTIFIER_H__

#include "_Org_Apache_Harmony_Security_X509_CSubjectKeyIdentifier.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CSubjectKeyIdentifier)
    , public Object
    , public ISubjectKeyIdentifier
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

    CARAPI GetKeyIdentifier(
        /* [out, callee] */ ArrayOf<Byte>** ppKeyIdentifier);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pKeyIdentifier);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CSUBJECTKEYIDENTIFIER_H__
