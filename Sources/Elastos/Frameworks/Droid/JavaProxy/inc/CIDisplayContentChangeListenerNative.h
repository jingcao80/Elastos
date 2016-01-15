
#ifndef __CIDISPLAYCONTENTCHANGELISTENERNATIVE_H__
#define __CIDISPLAYCONTENTCHANGELISTENERNATIVE_H__

#include "_CIDisplayContentChangeListenerNative.h"
#include <jni.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IWindowInfo;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIDisplayContentChangeListenerNative)
{
public:
    ~CIDisplayContentChangeListenerNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnWindowTransition(
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 transition,
        /* [in] */ IWindowInfo* info);

    CARAPI OnRectangleOnScreenRequested(
        /* [in] */ Int32 displayId,
        /* [in] */ IRect* rectangle,
        /* [in] */ Boolean immediate);

    CARAPI OnWindowLayersChanged(
        /* [in] */ Int32 displayId);

    CARAPI OnRotationChanged(
        /* [in] */ Int32 rotation);

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

#endif // __CIDISPLAYCONTENTCHANGELISTENERNATIVE_H__
