
#ifndef __ELASTOS_DROID_JAVAPROXY_CIWALLPAPERCONNECTIONNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIWALLPAPERCONNECTIONNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIWallpaperConnectionNative.h"
#include <jni.h>

using Elastos::Droid::Service::Wallpaper::IWallpaperEngine;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIWallpaperConnectionNative)
{
public:
    ~CIWallpaperConnectionNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI AttachEngine(
        /* [in] */ IWallpaperEngine* engine);

    CARAPI EngineShown(
        /* [in] */ IWallpaperEngine* engine);

    CARAPI SetWallpaper(
        /* [in] */ const String& name,
        /* [out] */ IParcelFileDescriptor** fd);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIWALLPAPERCONNECTIONNATIVE_H__
