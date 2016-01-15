
#ifndef __ELASTOS_DROID_APP_CFRAGMENTMANAGERHELPER_H__
#define __ELASTOS_DROID_APP_CFRAGMENTMANAGERHELPER_H__

#include "_Elastos_Droid_App_CFragmentManagerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CFragmentManagerHelper)
    , public Singleton
    , public IFragmentManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI EnableDebugLogging(
        /* [in] */ Boolean enabled);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CFRAGMENTMANAGERHELPER_H__
