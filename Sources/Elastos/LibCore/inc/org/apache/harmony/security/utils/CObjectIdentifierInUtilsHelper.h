
#ifndef __ORG_APACHE_HARMONY_SECURITY_UTILS_COBJECTIDENTIFIERINUTILSHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_UTILS_COBJECTIDENTIFIERINUTILSHELPER_H__

#include "_Org_Apache_Harmony_Security_Utils_CObjectIdentifierInUtilsHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

CarClass(CObjectIdentifierInUtilsHelper)
    , public Singleton
    , public IObjectIdentifierInUtilsHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI ValidateOid(
        /* [in] */ ArrayOf<Int32> * pOid);

    CARAPI HashIntArray(
        /* [in] */ ArrayOf<Int32> * pArray,
        /* [out] */ Int32 * pHash);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_UTILS_COBJECTIDENTIFIERINUTILSHELPER_H__
