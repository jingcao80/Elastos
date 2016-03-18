
#ifndef __ELASTOS_DROID_JAVAPROXY_CIIWALLPAPERSERVICENATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIIWALLPAPERSERVICENATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIIWallpaperServiceNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Service::Wallpaper::IIWallpaperConnection;
using Elastos::Droid::Service::Wallpaper::IIWallpaperService;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIIWallpaperServiceNative)
    , public Object
    , public IIWallpaperService
    , public IBinder
{
public:
    ~CIIWallpaperServiceNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI Attach(
        /* [in] */ IIWallpaperConnection* connection,
        /* [in] */ IBinder* windowToken,
        /* [in] */ Int32 windowType,
        /* [in] */ Boolean isPreview,
        /* [in] */ Int32 reqWidth,
        /* [in] */ Int32 reqHeight,
        /* [in] */ IRect* padding);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIIWALLPAPERSERVICENATIVE_H__
