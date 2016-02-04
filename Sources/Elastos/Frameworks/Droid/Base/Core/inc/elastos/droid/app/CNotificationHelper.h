
#ifndef __ELASTOS_DROID_APP_CNOTIFICATIONHELPER_H__
#define __ELASTOS_DROID_APP_CNOTIFICATIONHELPER_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Media.h"
#include "_Elastos_Droid_App_CNotificationHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Media::IAudioAttributes;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CNotificationHelper)
    , public Object
    , public INotificationHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetAUDIO_ATTRIBUTES_DEFAULT(
        /* [out] */ IAudioAttributes** attr);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CNOTIFICATIONHELPER_H__
