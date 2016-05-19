
#ifndef __ELASTOS_DROID_JAVAPROXY_CIWINDOWSESSIONCALLBACKNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIWINDOWSESSIONCALLBACKNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIWindowSessionCallbackNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::View::IIWindowSessionCallback;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIWindowSessionCallbackNative)
    , public Object
    , public IIWindowSessionCallback
    , public IBinder
{
public:
    ~CIWindowSessionCallbackNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnAnimatorScaleChanged(
        /* [in] */ Float scale);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIWINDOWSESSIONCALLBACKNATIVE_H__
