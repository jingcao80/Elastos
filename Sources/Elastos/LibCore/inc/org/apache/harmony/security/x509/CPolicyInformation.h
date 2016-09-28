
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CPOLICYINFORMATION_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CPOLICYINFORMATION_H__

#include "_Org_Apache_Harmony_Security_X509_CPolicyInformation.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CPolicyInformation)
    , public Object
    , public IPolicyInformation
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetPolicyIdentifier(
        /* [out] */ String* pPolicyIdentifier);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb);

    CARAPI constructor(
        /* [in] */ const String& policyIdentifier);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CPOLICYINFORMATION_H__
