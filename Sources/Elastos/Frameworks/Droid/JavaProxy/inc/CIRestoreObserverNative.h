
#ifndef __CIRESTOREOBSERVERNATIVE_H__
#define __CIRESTOREOBSERVERNATIVE_H__

#include "_CIRestoreObserverNative.h"
#include <jni.h>

using Elastos::Droid::App::Backup::IRestoreSet;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIRestoreObserverNative)
{
public:
    ~CIRestoreObserverNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

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

#endif // __CIRESTOREOBSERVERNATIVE_H__
