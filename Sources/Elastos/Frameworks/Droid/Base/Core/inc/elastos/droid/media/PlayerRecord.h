#ifndef _ELASTOS_DROID_MEDIA_PLAYERRECORD_H__
#define _ELASTOS_DROID_MEDIA_PLAYERRECORD_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentOnFinished;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::IBinder;
using Elastos::IO::IPrintWriter;

namespace Elastos{
namespace Droid {
namespace Media {
/**
 * @hide
 * Class to handle all the information about a media player, encapsulating information
 * about its use RemoteControlClient, playback type and volume... The lifecycle of each
 * instance is managed by android.media.MediaFocusControl, from its addition to the player stack
 * stack to its release.
 */
class PlayerRecord
	: public Object
	, public IPlayerRecord
	, public IProxyDeathRecipient
{
protected:
	class RccPlaybackState
        : public Object
        , public IPlayerRecordRccPlaybackState
	{
	public:
		RccPlaybackState();

        virtual ~RccPlaybackState();

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ Int32 state,
            /* [in] */ Int64 positionMs,
            /* [in] */ Float speed);

        CARAPI Reset();

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        CARAPI_(String) PosToString();

        CARAPI_(String) StateToString();

    public:
        Int32 mState;
        Int64 mPositionMs;
        Float mSpeed;
	};

	class RemotePlaybackState
        : public Object
        , public IPlayerRecordRemotePlaybackState
	{
	public:
        RemotePlaybackState();

        virtual ~RemotePlaybackState();

    protected:
        CARAPI constructor(
            /* [in] */ Int32 id,
            /* [in] */ Int32 vol,
            /* [in] */ Int32 volMax);
    public:
        Int32 mRccId;
        Int32 mVolume;
        Int32 mVolumeMax;
        Int32 mVolumeHandling;
	};

private:
    class RcClientDeathHandler
        : public Object
        , public IProxyDeathRecipient
    {
        friend class PlayerRecord;
    public:
        RcClientDeathHandler(
            /* [in] */ PlayerRecord* host,
            /* [in] */ IBinder* cb,
            /* [in] */ IPendingIntent* pi);

        ~RcClientDeathHandler();

        CAR_INTERFACE_DECL()



        CARAPI BinderDied();

        CARAPI GetBinder(
            /* [out] */ IBinder** result);

        CARAPI ProxyDied();

    private:
        AutoPtr<IBinder> mCb;
        AutoPtr<IPendingIntent> mMediaIntent;
        PlayerRecord* mHost;
    };

public:
	PlayerRecord();

	virtual ~PlayerRecord();

    CAR_INTERFACE_DECL()

	CARAPI Dump(
		/* [in] */ IPrintWriter* pw,
		/* [in] */ Boolean registrationInfo);

	CARAPI ResetPlaybackInfo();

	CARAPI UnlinkToRcClientDeath();

	CARAPI Destroy();

	CARAPI BinderDied();

protected:

	static CARAPI SetMediaFocusControl(
		/* [in] */ IPendingIntentOnFinished* mfc);

	CARAPI constructor(
		/* [in] */ IPendingIntent* mediaIntent,
		/* [in] */ IComponentName* eventReceiver,
		/* [in] */ IBinder* token);

	CARAPI GetRccId(
		/* [out] */ Int32* result);

	CARAPI GetRcc(
		/* [out] */ IIRemoteControlClient** result);

	CARAPI GetMediaButtonReceiver(
		/* [out] */ IComponentName** result);

	CARAPI GetMediaButtonIntent(
		/* [out] */ IPendingIntent** result);

	CARAPI HasMatchingMediaButtonIntent(
		/* [in] */ IPendingIntent* pi,
		/* [out] */ Boolean* result);

	CARAPI IsPlaybackActive(
		/* [out] */ Boolean* result);

	CARAPI ResetControllerInfoForRcc(
		/* [in] */ IIRemoteControlClient* rcClient,
        /* [in] */ const String& callingPackageName,
        /* [in] */ Int32 uid);

	CARAPI ResetControllerInfoForNoRcc();

	CARAPI Finalize();

public:
	static AutoPtr<IPendingIntentOnFinished> sController; //MediaFocusControl
    /**
     * Information only used for non-local playback
     */
    //FIXME private?
    Int32 mPlaybackType;
    Int32 mPlaybackVolume;
    Int32 mPlaybackVolumeMax;
    Int32 mPlaybackVolumeHandling;
    Int32 mPlaybackStream;
    AutoPtr<IPlayerRecordRccPlaybackState> mPlaybackState;
    AutoPtr<IIRemoteVolumeObserver> mRemoteVolumeObs;

private:
	// on purpose not using this classe's name, as it will only be used from MediaFocusControl
    static const String TAG;
    static const Boolean DEBUG;

    /**
     * A global counter for RemoteControlClient identifiers
     */
    static Int32 sLastRccId;

    /**
     * The target for the ACTION_MEDIA_BUTTON events.
     * Always non null. //FIXME verify
     */
    AutoPtr<IPendingIntent> mMediaIntent;
    /**
     * The registered media button event receiver.
     */
    AutoPtr<IComponentName> mReceiverComponent;

    Int32 mRccId;

    /**
     * A non-null token implies this record tracks a "live" player whose death is being monitored.
     */
    AutoPtr<IBinder> mToken;
    String mCallingPackageName;
    Int32 mCallingUid;
    /**
     * Provides access to the information to display on the remote control.
     * May be null (when a media button event receiver is registered,
     *     but no remote control client has been registered) */
    AutoPtr<IIRemoteControlClient> mRcClient;
    AutoPtr<RcClientDeathHandler> mRcClientDeathHandler;
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // _ELASTOS_DROID_MEDIA_PLAYERRECORD_H__
