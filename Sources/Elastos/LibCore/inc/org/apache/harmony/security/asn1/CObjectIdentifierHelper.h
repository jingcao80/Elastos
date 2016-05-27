
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_COBJECTIDENTIFIERHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_COBJECTIDENTIFIERHELPER_H__

#include "_Org_Apache_Harmony_Security_Asn1_CObjectIdentifierHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CObjectIdentifierHelper)
    , public Singleton
    , public IObjectIdentifierHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Validate(
        /* [in] */ ArrayOf<Int32>* oid);

    CARAPI ToString(
        /* [in] */ ArrayOf<Int32>* oid,
        /* [out] */ String* str);

    CARAPI ToIntArray(
        /* [in] */ const String& str,
        /* [out, callee] */ ArrayOf<Int32>** intArray);

    CARAPI IsOID(
        /* [in] */ const String& str,
        /* [out] */ Boolean* isOID);

private:
    // TODO: Add your private member variables here.
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_COBJECTIDENTIFIERHELPER_H__
