#ifndef __CDISPLAYMANAGERCALLBACKNATIVE_H__
#define __CDISPLAYMANAGERCALLBACKNATIVE_H__

#include "_CDisplayManagerCallbackNative.h"
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

#endif //__CDISPLAYMANAGERCALLBACKNATIVE_H__
