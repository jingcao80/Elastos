
#ifndef __CIFULLBACKUPRESTOREOBSERVERNATIVE_H__
#define __CIFULLBACKUPRESTOREOBSERVERNATIVE_H__

#include "_CIFullBackupRestoreObserverNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIFullBackupRestoreObserverNative)
{
public:
    ~CIFullBackupRestoreObserverNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnStartBackup();

    CARAPI OnBackupPackage(
        /* [in] */ const String& name);

    CARAPI OnEndBackup();

    CARAPI OnStartRestore();

    CARAPI OnRestorePackage(
        /* [in] */ const String& name);

    CARAPI OnEndRestore();

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

#endif // __CIFULLBACKUPRESTOREOBSERVERNATIVE_H__
