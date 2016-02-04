
#ifndef __ELASTOS_DROID_APP_CNOTIFICATIONBUILDERHELPER_H__
#define __ELASTOS_DROID_APP_CNOTIFICATIONBUILDERHELPER_H__

#include "_Elastos_Droid_App_CNotificationBuilderHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CNotificationBuilderHelper)
    , public Object
    , public INotificationBuilderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CalculateTopPadding(
        /* [in] */ IContext* ctx,
        /* [in] */ Boolean hasThreeLines,
        /* [in] */ Float fontScale,
        /* [out] */ Int32* result);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CNOTIFICATIONBUILDERHELPER_H__
