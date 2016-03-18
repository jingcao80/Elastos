
#ifndef __ELASTOS_DROID_JAVAPROXY_CIREMOTECONTROLCLIENT_H__
#define __ELASTOS_DROID_JAVAPROXY_CIREMOTECONTROLCLIENT_H__

#include "_Elastos_Droid_JavaProxy_CIRemoteControlClient.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Media::IIRemoteControlClient;
using Elastos::Droid::Media::IIRemoteControlDisplay;
using Elastos::Droid::Media::IRating;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIRemoteControlClient)
    , public Object
    , public IIRemoteControlClient
    , public IBinder
{
public:
    ~CIRemoteControlClient();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnInformationRequested(
        /* [in] */ Int32 generationId,
        /* [in] */ Int32 infoFlags);

    CARAPI InformationRequestForDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI SetCurrentClientGenerationId(
        /* [in] */ Int32 clientGeneration);

    CARAPI PlugRemoteControlDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI UnplugRemoteControlDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd);

    CARAPI SetBitmapSizeForDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI SetWantsSyncForDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Boolean wantsSync);

    CARAPI EnableRemoteControlDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Boolean enabled);

    CARAPI SeekTo(
        /* [in] */ Int32 clientGeneration,
        /* [in] */ Int64 timeMs);

    CARAPI UpdateMetadata(
        /* [in] */ Int32 clientGeneration,
        /* [in] */ Int32 key,
        /* [in] */ IRating* value);

    CARAPI SetPlayItem(
        /* [in] */ Int32 scope,
        /* [in] */ Int64 uid);

    CARAPI SetBrowsedPlayer();

    CARAPI GetNowPlayingEntries();

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIREMOTECONTROLCLIENT_H__
