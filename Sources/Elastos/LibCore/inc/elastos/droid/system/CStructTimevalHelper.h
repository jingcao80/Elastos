
#ifndef __ELASTOS_DROID_SYSTEM_CSTRUCTTIMEVALHELPER_H__
#define __ELASTOS_DROID_SYSTEM_CSTRUCTTIMEVALHELPER_H__

#include "core/Singleton.h"
#include "_Elastos_Droid_System_CStructTimevalHelper.h"


namespace Elastos {
namespace Droid {
namespace System {

CarClass(CStructTimevalHelper)
    , public Singleton
    , public IStructTimevalHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI FromMillis(
        /* [in] */ Int64 millis,
        /* [out] */ IStructTimeval** tv);
};

} // namespace System
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEM_CSTRUCTTIMEVALHELPER_H__
