#ifndef __ELASTOS_DROID_APP_CACTIVITYMANAGER_TASKDESCRITION_HELPER_H__
#define __ELASTOS_DROID_APP_CACTIVITYMANAGER_TASKDESCRITION_HELPER_H__

#include "_Elastos_Droid_App_CActivityManagerTaskDescriptionHelper.h"
#include "Elastos.Droid.Graphics.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CActivityManagerTaskDescriptionHelper)
    , public Singleton
    , public IActivityManagerTaskDescriptionHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI LoadTaskDescriptionIcon(
        /* [in] */ const String& iconFilename,
        /* [out] */ IBitmap** bm);
};

}// namespace App
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_APP_CACTIVITYMANAGER_TASKDESCRITION_HELPER_H__
