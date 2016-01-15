
#ifndef __CIRINGTONEPLAYER_H__
#define __CIRINGTONEPLAYER_H__

#include "_CIRingtonePlayer.h"
#include <jni.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IUserHandle;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIRingtonePlayer)
{
public:
    ~CIRingtonePlayer();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI Play(
        /* [in] */ IBinder* token,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 streamType);

    CARAPI Stop(
        /* [in] */ IBinder* token);

    CARAPI IsPlaying(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    CARAPI PlayAsync(
        /* [in] */ IUri* uri,
        /* [in] */ IUserHandle* user,
        /* [in] */ Boolean looping,
        /* [in] */ Int32 streamType);

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

#endif // __CIRINGTONEPLAYER_H__
