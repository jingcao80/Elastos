#ifndef __ELASTOS_DROID_JAVAPROXY_CDISPLAYMANAGERCALLBACKNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CDISPLAYMANAGERCALLBACKNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CDisplayManagerCallbackNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Hardware::Display::IIDisplayManagerCallback;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CDisplayManagerCallbackNative)
    , public Object
    , public IIDisplayManagerCallback
    , public IBinder
{
public:
    ~CDisplayManagerCallbackNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnDisplayEvent(
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 event);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

} // namespace JavaProxy
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_JAVAPROXY_CDISPLAYMANAGERCALLBACKNATIVE_H__
