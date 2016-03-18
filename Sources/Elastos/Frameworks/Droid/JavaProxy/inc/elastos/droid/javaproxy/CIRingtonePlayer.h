
#ifndef __ELASTOS_DROID_JAVAPROXY_CIRINGTONEPLAYER_H__
#define __ELASTOS_DROID_JAVAPROXY_CIRINGTONEPLAYER_H__

#include "_Elastos_Droid_JavaProxy_CIRingtonePlayer.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Media::IIRingtonePlayer;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IUserHandle;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIRingtonePlayer)
    , public Object
    , public IIRingtonePlayer
    , public IBinder
{
public:
    ~CIRingtonePlayer();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI Play(
        /* [in] */ IBinder* token,
        /* [in] */ IUri* uri,
        /* [in] */ IAudioAttributes* aa);

    CARAPI Stop(
        /* [in] */ IBinder* token);

    CARAPI IsPlaying(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    CARAPI SetVolume(
        /* [in] */ IBinder* token,
        /* [in] */ Float volume);

    CARAPI PlayAsync(
        /* [in] */ IUri* uri,
        /* [in] */ IUserHandle* user,
        /* [in] */ Boolean looping,
        /* [in] */ IAudioAttributes* aa);

    CARAPI StopAsync();

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIRINGTONEPLAYER_H__
