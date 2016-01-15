
#ifndef __ELASTOS_DROID_APP_CAPPOPS_MANAGER_APPOPS_CALLBACK_H__
#define __ELASTOS_DROID_APP_CAPPOPS_MANAGER_APPOPS_CALLBACK_H__

#include "_Elastos_Droid_App_CAppOpsManagerAppOpsCallback.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Internal::App::IIAppOpsCallback;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CAppOpsManagerAppOpsCallback)
    , public Object
    , public IIAppOpsCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IAppOpsManagerOnOpChangedListener* callback);

    CARAPI OpChanged(
        /* [in] */ Int32 op,
        /* [in] */ const String& packageName);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<IAppOpsManagerOnOpChangedListener> mCallback;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CAPPOPS_MANAGER_APPOPS_CALLBACK_H__