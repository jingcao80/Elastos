
#ifndef __ELASTOS_DROID_JAVAPROXY_CIREMOTECONTROLDISPLAY_H__
#define __ELASTOS_DROID_JAVAPROXY_CIREMOTECONTROLDISPLAY_H__

#include "_Elastos_Droid_JavaProxy_CIRemoteControlDisplay.h"
#include <jni.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIRemoteControlDisplay)
{
public:
    ~CIRemoteControlDisplay();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI GetJavaInstance(
        /* [out] */ Handle32* obj);

    CARAPI SetCurrentClientId(
        /* [in] */ Int32 clientGeneration,
        /* [in] */ IPendingIntent* clientMediaIntent,
        /* [in] */ Boolean clearing);

    CARAPI SetPlaybackState(
        /* [in] */ Int32 generationId,
        /* [in] */ Int32 state,
        /* [in] */ Int64 stateChangeTimeMs);

    CARAPI SetTransportControlFlags(
        /* [in] */ Int32 generationId,
        /* [in] */ Int32 transportControlFlags);

    CARAPI SetMetadata(
        /* [in] */ Int32 generationId,
        /* [in] */ IBundle* metadata);

    CARAPI SetArtwork(
        /* [in] */ Int32 generationId,
        /* [in] */ IBitmap* artwork);

    CARAPI SetAllMetadata(
        /* [in] */ Int32 generationId,
        /* [in] */ IBundle* metadata,
        /* [in] */ IBitmap* artwork);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIREMOTECONTROLDISPLAY_H__
