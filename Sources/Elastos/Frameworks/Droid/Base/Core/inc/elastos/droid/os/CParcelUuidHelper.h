
#ifndef __ELASTOS_DROID_OS_CPARCELUUIDHELPER_H__
#define __ELASTOS_DROID_OS_CPARCELUUIDHELPER_H__

#include "_Elastos_Droid_Os_CParcelUuidHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CParcelUuidHelper)
    , public Singleton
    , public IParcelUuidHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI FromString(
        /* [in] */ const String& uuid,
        /* [out] */ IParcelUuid** id);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CPARCELUUIDHELPER_H__
