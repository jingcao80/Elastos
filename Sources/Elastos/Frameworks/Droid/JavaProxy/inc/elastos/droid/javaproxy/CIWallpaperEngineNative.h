
#ifndef __ELASTOS_DROID_JAVAPROXY_CIWALLPAPERENGINENATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIWALLPAPERENGINENATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIWallpaperEngineNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Service::Wallpaper::IIWallpaperEngine;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIWallpaperEngineNative)
    , public Object
    , public IIWallpaperEngine
    , public IBinder
{
public:
    ~CIWallpaperEngineNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI SetDesiredSize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI SetDisplayPadding(
        /* [in] */ IRect* padding);

    CARAPI SetVisibility(
        /* [in] */ Boolean visible);

    CARAPI DispatchPointer(
        /* [in] */ IMotionEvent* event);

    CARAPI DispatchWallpaperCommand(
        /* [in] */ const String& action,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ IBundle* extras);

    CARAPI Destroy();

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIWALLPAPERENGINENATIVE_H__
