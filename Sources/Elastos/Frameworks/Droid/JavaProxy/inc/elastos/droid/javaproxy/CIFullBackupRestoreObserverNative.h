
#ifndef __ELASTOS_DROID_JAVAPROXY_CIFULLBACKUPRESTOREOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIFULLBACKUPRESTOREOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIFullBackupRestoreObserverNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::App::Backup::IIFullBackupRestoreObserver;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIFullBackupRestoreObserverNative)
    , public Object
    , public IIFullBackupRestoreObserver
    , public IBinder
{
public:
    ~CIFullBackupRestoreObserverNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnStartBackup();

    CARAPI OnBackupPackage(
        /* [in] */ const String& name);

    CARAPI OnEndBackup();

    CARAPI OnEndBackupWithResult(
        /* [in] */ Int32 result);

    CARAPI OnStartRestore();

    CARAPI OnRestorePackage(
        /* [in] */ const String& name);

    CARAPI OnEndRestore();

    CARAPI OnEndRestoreWithResult(
        /* [in] */ Int32 result);

    CARAPI OnTimeout();

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIFULLBACKUPRESTOREOBSERVERNATIVE_H__
