#ifndef __ELASTOS_DROID_SYSTEMUI_MEDIA_RINGTONEPLAYER_H__
#define __ELASTOS_DROID_SYSTEMUI_MEDIA_RINGTONEPLAYER_H__

#include "_SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/systemui/SystemUI.h"
#include "elastos/droid/systemui/media/NotificationPlayer.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Media::IIAudioService;
using Elastos::Droid::Media::IIRingtonePlayer;
using Elastos::Droid::Media::IRingtone;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::SystemUI::SystemUI;
using Elastos::Droid::SystemUI::Media::NotificationPlayer;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Media{

/**
 * Service that offers to play ringtones by {@link Uri}, since our process has
 * {@link android.Manifest.permission#READ_EXTERNAL_STORAGE}.
 */
class RingtonePlayer
    : public SystemUI
{
private:
    class Client
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        Client(
            /* [in] */ IBinder* token,
            /* [in] */ IUri* uri,
            /* [in] */ IUserHandle* user,
            /* [in] */ IAudioAttributes* aa,
            /* [in] */ RingtonePlayer* host);

        ProxyDied();

    public:
        AutoPtr<IBinder> mToken;
        AutoPtr<IRingtone> mRingtone;
        RingtonePlayer* mHost;
    };

    class MyRingtonePlayer
        : public Object
        , public IIRingtonePlayer
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MyRingtonePlayer(
            /* [in] */ RingtonePlayer* host);

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
        CARAPI SetVolume(
            /* [in] */ IBinder* token,
            /* [in] */ Float volume);

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

    private:
        RingtonePlayer* mHost;
    };

public:
    RingtonePlayer();

    CARAPI constructor();

    //@Override
    CARAPI Start();

private:
    CARAPI_(AutoPtr<IContext>) GetContextForUser(
        /* [in] */ IUserHandle* user);

private:
    static const String TAG;
    static const Boolean LOGD;

    // TODO: support Uri switching under same IBinder

    AutoPtr<IIAudioService> mAudioService;

    AutoPtr<NotificationPlayer> mAsyncPlayer; // = new NotificationPlayer(TAG);
    AutoPtr<IHashMap> mClients; // = Maps.newHashMap();

    AutoPtr<MyRingtonePlayer> mCallback;
};

} // namespace Media
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_MEDIA_RINGTONEPLAYER_H__
