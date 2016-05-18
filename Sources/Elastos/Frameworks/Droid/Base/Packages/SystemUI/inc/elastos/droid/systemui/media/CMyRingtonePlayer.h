#ifndef __ELASTOS_DROID_SYSTEMUI_MEDIA_CMYRINGTONEPLAYER_H__
#define __ELASTOS_DROID_SYSTEMUI_MEDIA_CMYRINGTONEPLAYER_H__

#include "_Elastos_Droid_SystemUI_Media_CMyRingtonePlayer.h"
#include "elastos/droid/systemui/media/CRingtonePlayer.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Media{

class CRingtonePlayer;

CarClass(CMyRingtonePlayer)
    , public Object
    , public IIRingtonePlayer
    , public IBinder
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ISystemUI* host);

    //@Override
    CARAPI Play(
        /* [in] */ IBinder* token,
        /* [in] */ IUri* uri,
        /* [in] */ IAudioAttributes* aa);

    //@Override
    CARAPI Stop(
        /* [in] */ IBinder* token);

    //@Override
    CARAPI IsPlaying(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI PlayAsync(
        /* [in] */ IUri* uri,
        /* [in] */ IUserHandle* user,
        /* [in] */ Boolean looping,
        /* [in] */ IAudioAttributes* aa);

    //@Override
    CARAPI StopAsync();

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI SetVolume(
        /* [in] */ IBinder* token,
        /* [in] */ Float volume);

private:
    CRingtonePlayer* mHost;
};


} // namespace Media
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_MEDIA_CMYRINGTONEPLAYER_H__
