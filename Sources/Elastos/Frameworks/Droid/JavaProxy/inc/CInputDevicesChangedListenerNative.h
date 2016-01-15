
#ifndef __CINPUTDEVICESCHANGEDLISTENERNATIVE_H__
#define __CINPUTDEVICESCHANGEDLISTENERNATIVE_H__

#include "_CInputDevicesChangedListenerNative.h"
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

#endif // __CINPUTDEVICESCHANGEDLISTENERNATIVE_H__
