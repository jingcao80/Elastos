
#ifndef __ELASTOS_DROID_JAVAPROXY_CINPUTDEVICESCHANGEDLISTENERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CINPUTDEVICESCHANGEDLISTENERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CInputDevicesChangedListenerNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Hardware::Input::IInputDevicesChangedListener;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CInputDevicesChangedListenerNative)
    , public Object
    , public IInputDevicesChangedListener
    , public IBinder
{
public:
    ~CInputDevicesChangedListenerNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnInputDevicesChanged(
        /* [in] */ ArrayOf<Int32>* deviceIdAndGeneration);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CINPUTDEVICESCHANGEDLISTENERNATIVE_H__
