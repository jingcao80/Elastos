
#ifndef __ELASTOS_DROID_JAVAPROXY_CIREMOTECONTROLDISPLAY_H__
#define __ELASTOS_DROID_JAVAPROXY_CIREMOTECONTROLDISPLAY_H__

#include "_Elastos_Droid_JavaProxy_CIRemoteControlDisplay.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Media::IIRemoteControlDisplay;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIRemoteControlDisplay)
    , public Object
    , public IIRemoteControlDisplay
    , public IJavaNativeProxy
    , public IBinder
{
public:
    ~CIRemoteControlDisplay();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI GetJavaInstance(
        /* [out] */ Handle64* obj);

    CARAPI SetCurrentClientId(
        /* [in] */ Int32 clientGeneration,
        /* [in] */ IPendingIntent* clientMediaIntent,
        /* [in] */ Boolean clearing);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI SetPlaybackState(
        /* [in] */ Int32 generationId,
        /* [in] */ Int32 state,
        /* [in] */ Int64 stateChangeTimeMs,
        /* [in] */ Int64 currentPosMs,
        /* [in] */ Float speed);

    CARAPI SetTransportControlInfo(
        /* [in] */ Int32 generationId,
        /* [in] */ Int32 transportControlFlags,
        /* [in] */ Int32 posCapabilities);

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
