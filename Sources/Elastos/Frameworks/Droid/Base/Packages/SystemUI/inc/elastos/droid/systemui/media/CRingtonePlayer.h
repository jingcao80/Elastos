#ifndef __ELASTOS_DROID_SYSTEMUI_MEDIA_CRINGTONEPLAYER_H__
#define __ELASTOS_DROID_SYSTEMUI_MEDIA_CRINGTONEPLAYER_H__

#include "_Elastos_Droid_SystemUI_Media_CRingtonePlayer.h"
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
CarClass(CRingtonePlayer)
    , public SystemUI
{
public:
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
            /* [in] */ CRingtonePlayer* host);

        ProxyDied();

    public:
        AutoPtr<IBinder> mToken;
        AutoPtr<IRingtone> mRingtone;
        CRingtonePlayer* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CRingtonePlayer();

    //@Override
    CARAPI Start();

    CARAPI_(AutoPtr<IContext>) GetContextForUser(
        /* [in] */ IUserHandle* user);

public:
    static const String TAG;
    static const Boolean LOGD;

    // TODO: support Uri switching under same IBinder

    AutoPtr<IIAudioService> mAudioService;

    AutoPtr<NotificationPlayer> mAsyncPlayer;
    AutoPtr<IHashMap> mClients;

    AutoPtr<IIRingtonePlayer> mCallback;
};

} // namespace Media
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_MEDIA_CRINGTONEPLAYER_H__
