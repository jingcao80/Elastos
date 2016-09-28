
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CINFOACCESSSYNTAX_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CINFOACCESSSYNTAX_H__

#include "_Org_Apache_Harmony_Security_X509_CInfoAccessSyntax.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CInfoAccessSyntax)
    , public Object
    , public IInfoAccessSyntax
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

    CARAPI ToString(
        /* [out] */ String* pStr);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CINFOACCESSSYNTAX_H__
