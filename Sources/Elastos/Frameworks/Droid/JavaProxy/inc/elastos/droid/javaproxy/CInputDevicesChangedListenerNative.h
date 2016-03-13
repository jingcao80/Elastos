
#ifndef __ELASTOS_DROID_JAVAPROXY_CINPUTDEVICESCHANGEDLISTENERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CINPUTDEVICESCHANGEDLISTENERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CInputDevicesChangedListenerNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CInputDevicesChangedListenerNative)
{
public:
    ~CInputDevicesChangedListenerNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnInputDevicesChanged(
        /* [in] */ ArrayOf<Int32>* deviceIdAndGeneration);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CINPUTDEVICESCHANGEDLISTENERNATIVE_H__
