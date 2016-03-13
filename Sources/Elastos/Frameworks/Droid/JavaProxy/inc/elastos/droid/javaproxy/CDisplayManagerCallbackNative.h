#ifndef __ELASTOS_DROID_JAVAPROXY_CDISPLAYMANAGERCALLBACKNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CDISPLAYMANAGERCALLBACKNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CDisplayManagerCallbackNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CDisplayManagerCallbackNative)
{
public:
    ~CDisplayManagerCallbackNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnDisplayEvent(
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 event);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

} // namespace JavaProxy
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_JAVAPROXY_CDISPLAYMANAGERCALLBACKNATIVE_H__
