
#ifndef __CMOUNTSERVICELISTENER_H__
#define __CMOUNTSERVICELISTENER_H__

#include "_CMountServiceListener.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CMountServiceListener)
{
public:
    ~CMountServiceListener();
    
    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnUsbMassStorageConnectionChanged(
        /* [in] */ Boolean connected);

    CARAPI OnStorageStateChanged(
        /* [in] */ const String& path,
        /* [in] */ const String& oldState,
        /* [in] */ const String& newState);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __CMOUNTSERVICELISTENER_H__
