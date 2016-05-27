
#ifndef __ORG_APACHE_HARMONY_SECURITY_UTILS_COBJECTIDENTIFIERINUTILS_H__
#define __ORG_APACHE_HARMONY_SECURITY_UTILS_COBJECTIDENTIFIERINUTILS_H__

#include "_Org_Apache_Harmony_Security_Utils_CObjectIdentifierInUtils.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

CarClass(CObjectIdentifierInUtils)
    , public Object
    , public IObjectIdentifierInUtils
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetOid(
        /* [out, callee] */ ArrayOf<Int32> ** ppOid);

    CARAPI GetName(
        /* [out] */ String * pName);

    CARAPI GetGroup(
        /* [out] */ IObject ** ppGroup);

    CARAPI Equals(
        /* [in] */ IInterface * pO,
        /* [out] */ Boolean * pResult);

    CARAPI ToOIDString(
        /* [out] */ String * pStr);

    CARAPI ToString(
        /* [out] */ String * pStr);

    CARAPI GetHashCode(
        /* [out] */ Int32 * pHashCode);

    CARAPI constructor(
        /* [in] */ ArrayOf<Int32> * pOid);

    CARAPI constructor(
        /* [in] */ ArrayOf<Int32> * pOid,
        /* [in] */ const String& name,
        /* [in] */ IObject * pOidGroup);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_UTILS_COBJECTIDENTIFIERINUTILS_H__
