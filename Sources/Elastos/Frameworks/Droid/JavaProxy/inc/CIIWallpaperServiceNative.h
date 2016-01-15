
#ifndef __CIIWALLPAPERSERVICENATIVE_H__
#define __CIIWALLPAPERSERVICENATIVE_H__

#include "_CIIWallpaperServiceNative.h"
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Service::Wallpaper::IWallpaperConnection;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIIWallpaperServiceNative)
{
public:
    ~CIIWallpaperServiceNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI Attach(
        /* [in] */ IWallpaperConnection* connection,
        /* [in] */ IBinder* windowToken,
        /* [in] */ Int32 windowType,
        /* [in] */ Boolean isPreview,
        /* [in] */ Int32 reqWidth,
        /* [in] */ Int32 reqHeight);

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

#endif // __CIIWALLPAPERSERVICENATIVE_H__
