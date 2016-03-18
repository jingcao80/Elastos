
#ifndef __ELASTOS_DROID_JAVAPROXY_CIRESTOREOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIRESTOREOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIRestoreObserverNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::App::Backup::IIRestoreObserver;
using Elastos::Droid::App::Backup::IRestoreSet;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIRestoreObserverNative)
    , public Object
    , public IIRestoreObserver
    , public IBinder
{
public:
    ~CIRestoreObserverNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI RestoreSetsAvailable(
        /* [in] */ ArrayOf<IRestoreSet*>* results);

    CARAPI RestoreStarting(
        /* [in] */ Int32 numPackages);

    CARAPI OnUpdate(
        /* [in] */ Int32 nowBeingRestored,
        /* [in] */ const String& curentPackage);

    CARAPI RestoreFinished(
        /* [in] */ Int32 error);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIRESTOREOBSERVERNATIVE_H__
