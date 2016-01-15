
#ifndef __ELASTOS_DROID_APP_CACTIVITYOPTIONSANIMATIONSTARTEDLISTENER_H__
#define __ELASTOS_DROID_APP_CACTIVITYOPTIONSANIMATIONSTARTEDLISTENER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_App_CActivityOptionsAnimationStartedListener.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IRemoteCallback;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CActivityOptionsAnimationStartedListener)
    , public Object
    , public IRemoteCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IHandler* handler,
        /* [in] */ IActivityOptionsOnAnimationStartedListener* listener);

    CARAPI SendResult(
        /* [in] */ IBundle* data);

    CARAPI ToString(
        /* [out]*/ String* str);

private:
    AutoPtr<IHandler> mHandler;
    AutoPtr<IActivityOptionsOnAnimationStartedListener> mListener;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CACTIVITYOPTIONSANIMATIONSTARTEDLISTENER_H__
