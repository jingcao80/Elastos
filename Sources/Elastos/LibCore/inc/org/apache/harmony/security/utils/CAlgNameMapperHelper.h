
#ifndef __ORG_APACHE_HARMONY_SECURITY_UTILS_CALGNAMEMAPPERHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_UTILS_CALGNAMEMAPPERHELPER_H__

#include "_Org_Apache_Harmony_Security_Utils_CAlgNameMapperHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

CarClass(CAlgNameMapperHelper)
    , public Singleton
    , public IAlgNameMapperHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Map2OID(
        /* [in] */ const String& algName,
        /* [out] */ String * pOid);

    CARAPI Map2AlgName(
        /* [in] */ const String& oid,
        /* [out] */ String * pAlgName);

    CARAPI GetStandardName(
        /* [in] */ const String& algName,
        /* [out] */ String * pStandardName);

    CARAPI IsOID(
        /* [in] */ const String& alias,
        /* [out] */ Boolean * pIsOid);

    CARAPI Normalize(
        /* [in] */ const String& oid,
        /* [out] */ String * pNormalString);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_UTILS_CALGNAMEMAPPERHELPER_H__
