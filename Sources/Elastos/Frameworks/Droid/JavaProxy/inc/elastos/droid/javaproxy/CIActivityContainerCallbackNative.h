
#ifndef __ELASTOS_DROID_JAVAPROXY_CIACTIVITYCONTAINERCALLBACKNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIACTIVITYCONTAINERCALLBACKNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIActivityContainerCallbackNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::App::IActivityContainerCallback;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIActivityContainerCallbackNative)
    , public Object
    , public IActivityContainerCallback
    , public IBinder
{
public:
    ~CIActivityContainerCallbackNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI SetVisible(
        /* [in] */ IBinder* container,
        /* [in] */ Boolean visible);

    CARAPI OnAllActivitiesComplete(
        /* [in] */ IBinder* container);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_JAVAPROXY_CIACTIVITYCONTAINERCALLBACKNATIVE_H__
