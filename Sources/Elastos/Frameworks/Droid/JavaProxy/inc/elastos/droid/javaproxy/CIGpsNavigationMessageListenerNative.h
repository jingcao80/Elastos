
#ifndef __ELASTOS_DROID_JAVAPROXY_CIGPSNAVIGATIONMESSAGELISTENERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIGPSNAVIGATIONMESSAGELISTENERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIGpsNavigationMessageListenerNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Location::IIGpsNavigationMessageListener;
using Elastos::Droid::Location::IGpsNavigationMessageEvent;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIGpsNavigationMessageListenerNative)
    , public Object
    , public IIGpsNavigationMessageListener
    , public IBinder
{
public:
    ~CIGpsNavigationMessageListenerNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnGpsNavigationMessageReceived(
        /* [in] */ IGpsNavigationMessageEvent* event);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIGPSNAVIGATIONMESSAGELISTENERNATIVE_H__
