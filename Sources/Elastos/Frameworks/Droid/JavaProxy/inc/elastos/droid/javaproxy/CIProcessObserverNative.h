
#ifndef __ELASTOS_DROID_JAVAPROXY_CIPROCESSOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIPROCESSOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIProcessObserverNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::App::IIProcessObserver;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIProcessObserverNative)
    , public Object
    , public IIProcessObserver
    , public IBinder
{
public:
    ~CIProcessObserverNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnForegroundActivitiesChanged(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean foregroundActivities);

    CARAPI OnProcessStateChanged(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 importance);

    CARAPI OnProcessDied(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CIPROCESSOBSERVERNATIVE_H__
