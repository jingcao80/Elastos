#include "elastos/droid/media/PlayerRecord.h"
#include "elastos/droid/media/MediaFocusControl.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos{
namespace Droid {
namespace Media {
//================================================================================
//                      PlayerRecord::RccPlaybackState
//================================================================================
PlayerRecord::RccPlaybackState::RccPlaybackState()
    : mState(0)
    , mPositionMs(0)
    , mSpeed(0)
{}

PlayerRecord::RccPlaybackState::~RccPlaybackState()
{}

CAR_INTERFACE_IMPL(PlayerRecord::RccPlaybackState, Object, IPlayerRecordRccPlaybackState)

ECode PlayerRecord::RccPlaybackState::constructor(
    /* [in] */ Int32 state,
    /* [in] */ Int64 positionMs,
    /* [in] */ Float speed)
{
    mState = state;
    mPositionMs = positionMs;
    mSpeed = speed;
    return NOERROR;
}

ECode PlayerRecord::RccPlaybackState::Reset()
{
    mState = IRemoteControlClient::PLAYSTATE_STOPPED;
    mPositionMs = IRemoteControlClient::PLAYBACK_POSITION_INVALID;
    mSpeed = IRemoteControlClient::PLAYBACK_SPEED_1X;
    return NOERROR;
}

ECode PlayerRecord::RccPlaybackState::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder sb(StateToString());
    sb += ", "; sb += PosToString(); sb += ", "; sb += mSpeed; sb += "X";
    *result = sb.ToString();
    return NOERROR;
}

String PlayerRecord::RccPlaybackState::PosToString()
{
    if (mPositionMs == IRemoteControlClient::PLAYBACK_POSITION_INVALID) {
        return String("PLAYBACK_POSITION_INVALID");
    } else if (mPositionMs == IRemoteControlClient::PLAYBACK_POSITION_ALWAYS_UNKNOWN) {
        return String("PLAYBACK_POSITION_ALWAYS_UNKNOWN");
    } else {
        return StringUtils::ToString(mPositionMs) + String("ms");
    }
}

String PlayerRecord::RccPlaybackState::StateToString()
{
    switch (mState) {
        case IRemoteControlClient::PLAYSTATE_NONE:
            return String("PLAYSTATE_NONE");
        case IRemoteControlClient::PLAYSTATE_STOPPED:
            return String("PLAYSTATE_STOPPED");
        case IRemoteControlClient::PLAYSTATE_PAUSED:
            return String("PLAYSTATE_PAUSED");
        case IRemoteControlClient::PLAYSTATE_PLAYING:
            return String("PLAYSTATE_PLAYING");
        case IRemoteControlClient::PLAYSTATE_FAST_FORWARDING:
            return String("PLAYSTATE_FAST_FORWARDING");
        case IRemoteControlClient::PLAYSTATE_REWINDING:
            return String("PLAYSTATE_REWINDING");
        case IRemoteControlClient::PLAYSTATE_SKIPPING_FORWARDS:
            return String("PLAYSTATE_SKIPPING_FORWARDS");
        case IRemoteControlClient::PLAYSTATE_SKIPPING_BACKWARDS:
            return String("PLAYSTATE_SKIPPING_BACKWARDS");
        case IRemoteControlClient::PLAYSTATE_BUFFERING:
            return String("PLAYSTATE_BUFFERING");
        case IRemoteControlClient::PLAYSTATE_ERROR:
            return String("PLAYSTATE_ERROR");
        default:
            return String("[invalid playstate]");
    }
}

//================================================================================
//                      PlayerRecord::RemotePlaybackState
//================================================================================

CAR_INTERFACE_IMPL(PlayerRecord::RemotePlaybackState, Object, IPlayerRecordRemotePlaybackState)

PlayerRecord::RemotePlaybackState::RemotePlaybackState()
    : mRccId(0)
    , mVolume(0)
    , mVolumeMax(0)
    , mVolumeHandling(0)
{}

PlayerRecord::RemotePlaybackState::~RemotePlaybackState()
{}

ECode PlayerRecord::RemotePlaybackState::constructor(
    /* [in] */ Int32 id,
    /* [in] */ Int32 vol,
    /* [in] */ Int32 volMax)
{
    mRccId = id;
    mVolume = vol;
    mVolumeMax = volMax;
    mVolumeHandling = IRemoteControlClient::DEFAULT_PLAYBACK_VOLUME_HANDLING;
    return NOERROR;
}

//================================================================================
//                      PlayerRecord::RcClientDeathHandler
//================================================================================
PlayerRecord::RcClientDeathHandler::RcClientDeathHandler(
    /* [in] */ PlayerRecord* host,
    /* [in] */ IBinder* cb,
    /* [in] */ IPendingIntent* pi)
{
    mHost = host;
    mCb = cb;
    mMediaIntent = pi;
}

PlayerRecord::RcClientDeathHandler::~RcClientDeathHandler()
{}

CAR_INTERFACE_IMPL(PlayerRecord::RcClientDeathHandler, Object, IProxyDeathRecipient)

ECode PlayerRecord::RcClientDeathHandler::BinderDied()
{
    Slogger::W(TAG, "  RemoteControlClient died");
    // remote control client died, make sure the displays don't use it anymore
    //  by setting its remote control client to null
    ((MediaFocusControl*)((mHost->sController).Get()))->RegisterRemoteControlClient(mMediaIntent.Get(), NULL/*rcClient*/, String(NULL)/*ignored*/);
    // the dead client was maybe handling remote playback, the controller should reevaluate
    ((MediaFocusControl*)((mHost->sController).Get()))->PostReevaluateRemote();
    return NOERROR;
}

ECode PlayerRecord::RcClientDeathHandler::GetBinder(
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCb;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode PlayerRecord::RcClientDeathHandler::ProxyDied()
{
    return NOERROR;
}

//================================================================================
//                      PlayerRecord
//================================================================================
const String PlayerRecord::TAG (String("MediaFocusControl"));
const Boolean PlayerRecord::DEBUG = FALSE;
Int32 PlayerRecord::sLastRccId = 0;
AutoPtr<IMediaFocusControl> PlayerRecord::sController;

PlayerRecord::PlayerRecord()
    : mPlaybackType(0)
    , mPlaybackVolume(0)
    , mPlaybackVolumeMax(0)
    , mPlaybackVolumeHandling(0)
    , mPlaybackStream(0)
    , mRccId(-1)
    , mCallingUid(0)
{}

PlayerRecord::~PlayerRecord()
{}

CAR_INTERFACE_IMPL(PlayerRecord, Object, IProxyDeathRecipient);

ECode PlayerRecord::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Boolean registrationInfo)
{
    // if (registrationInfo) {
    //     pw->Println(String("  pi: "));
    //     pw->Println(mMediaIntent.Get());
    //     pw->Println(String(" -- pack: ") + mCallingPackageName +
    //                 String("  -- ercvr: "));
    //     pw->Println(mReceiverComponent.Get());
    //     pw->Println(String("  -- client: "));
    //     pw->Println(mRcClient.Get());
    //     pw->Println(String("  -- uid: ") + mCallingUid +
    //                 String("  -- type: ") + mPlaybackType +
    //                 String("  state: " ));
    //     pw->Println(mPlaybackState.Get());
    // } else {
    //     // emphasis on state
    //     pw->Println(String("  uid: ") + mCallingUid +
    //                 String("  -- id: ") + mRccId +
    //                 String("  -- type: ") + mPlaybackType +
    //                 String("  -- state: "));
    //     pw->Println(mPlaybackState.Get());
    //     pw->Println(String("  -- vol handling: ") + mPlaybackVolumeHandling +
    //                 String("  -- vol: ") + mPlaybackVolume +
    //                 String("  -- volMax: ") + mPlaybackVolumeMax +
    //                 String("  -- volObs: "));
    //     pw->Println(mRemoteVolumeObs.Get());
    // }
    return NOERROR;
}

ECode PlayerRecord::ResetPlaybackInfo()
{
    mPlaybackType = IRemoteControlClient::PLAYBACK_TYPE_LOCAL;
    mPlaybackVolume = IRemoteControlClient::DEFAULT_PLAYBACK_VOLUME;
    mPlaybackVolumeMax = IRemoteControlClient::DEFAULT_PLAYBACK_VOLUME;
    mPlaybackVolumeHandling = IRemoteControlClient::DEFAULT_PLAYBACK_VOLUME_HANDLING;
    mPlaybackStream = IAudioManager::STREAM_MUSIC;
    mPlaybackState->Reset();
    mRemoteVolumeObs = NULL;
    return NOERROR;
}

ECode PlayerRecord::UnlinkToRcClientDeath()
{
    if ((mRcClientDeathHandler != NULL) && (mRcClientDeathHandler->mCb != NULL)) {
        // try {
        AutoPtr<IBinder> cb = mRcClientDeathHandler->mCb;
        AutoPtr<IProxy> proxy = (IProxy*)cb->Probe(EIID_IProxy);
        assert(proxy != NULL);
        Boolean flag = FALSE;
        proxy->UnlinkToDeath((IProxyDeathRecipient*)mRcClientDeathHandler.Get(), 0, &flag);
        mRcClientDeathHandler = NULL;
        // } catch (java.util.NoSuchElementException e) {
            // not much we can do here
            // Log.e(TAG, "Error in unlinkToRcClientDeath()", e);
        // }
    }
    return NOERROR;
}

ECode PlayerRecord::Destroy()
{
    UnlinkToRcClientDeath();
    if (mToken != NULL) {
        AutoPtr<IProxy> proxy = (IProxy*)mToken->Probe(EIID_IProxy);
        assert(proxy != NULL);
        Boolean flag = FALSE;
        proxy->UnlinkToDeath(this, 0, &flag);
        mToken = NULL;
    }
    return NOERROR;
}

ECode PlayerRecord::ProxyDied()
{
    ((MediaFocusControl*)(sController.Get()))->UnregisterMediaButtonIntentAsync(mMediaIntent);
    return NOERROR;
}

ECode PlayerRecord::SetMediaFocusControl(
    /* [in] */ IMediaFocusControl* mfc)
{
    sController = mfc;
    return NOERROR;
}

ECode PlayerRecord::constructor(
    /* [in] */ IPendingIntent* mediaIntent,
    /* [in] */ IComponentName* eventReceiver,
    /* [in] */ IBinder* token)
{
    ECode ec = NOERROR;
    mMediaIntent = mediaIntent;
    mReceiverComponent = eventReceiver;
    mToken = token;
    mCallingUid = -1;
    mRcClient = NULL;
    mRccId = ++sLastRccId;
    AutoPtr<RccPlaybackState> rp = new RccPlaybackState();
    rp->constructor(
        IRemoteControlClient::PLAYSTATE_STOPPED,
        IRemoteControlClient::PLAYBACK_POSITION_INVALID,
        IRemoteControlClient::PLAYBACK_SPEED_1X);
    ResetPlaybackInfo();
    if (mToken != NULL) {
        // try {
         AutoPtr<IProxy> proxy = (IProxy*)mToken->Probe(EIID_IProxy);
        assert(proxy != NULL);
        Boolean flag = FALSE;
        proxy->LinkToDeath(this, 0);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            ((MediaFocusControl*)sController.Get())->UnregisterMediaButtonIntentAsync(mMediaIntent);
            return NOERROR;
        }
        // } catch (RemoteException e) {
            // sController.unregisterMediaButtonIntentAsync(mMediaIntent);
        // }
    }
    return NOERROR;
}

ECode PlayerRecord::GetRccId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRccId;
    return NOERROR;
}

ECode PlayerRecord::GetRcc(
    /* [out] */ IIRemoteControlClient** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRcClient;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode PlayerRecord::GetMediaButtonReceiver(
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mReceiverComponent;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode PlayerRecord::GetMediaButtonIntent(
    /* [out] */ IPendingIntent** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMediaIntent;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PlayerRecord::HasMatchingMediaButtonIntent(
    /* [in] */ IPendingIntent* pi,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mToken != NULL) {
        *result = mMediaIntent.Get() == pi;
        return NOERROR;
        if (mReceiverComponent != NULL) {
            AutoPtr<IIntent> intent;
            pi->GetIntent((IIntent**)&intent);
            AutoPtr<IComponentName> componentName;
            intent->GetComponent((IComponentName**)&componentName);
            *result = mReceiverComponent == componentName;
            // *result = IObject::Equals(TO_IINTERFACE(mReceiverComponent), TO_IINTERFACE(componentName));
            return NOERROR;
        } else {
            *result = FALSE;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode PlayerRecord::IsPlaybackActive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = MediaFocusControl::IsPlaystateActive(((RccPlaybackState*)(mPlaybackState.Get()))->mState);
    return NOERROR;
}

ECode PlayerRecord::ResetControllerInfoForRcc(
    /* [in] */ IIRemoteControlClient* rcClient,
    /* [in] */ const String& callingPackageName,
    /* [in] */ Int32 uid)
{
    // already had a remote control client?
    if (mRcClientDeathHandler != NULL) {
        // stop monitoring the old client's death
        UnlinkToRcClientDeath();
    }
    // save the new remote control client
    mRcClient = rcClient;
    mCallingPackageName = callingPackageName;
    mCallingUid = uid;
    if (rcClient == NULL) {
        // here mcse.mRcClientDeathHandler is NULL;
        return ResetPlaybackInfo();
    }
    else {
        AutoPtr<IBinder> b = IBinder::Probe(mRcClient);
        AutoPtr<RcClientDeathHandler> rcdh = new RcClientDeathHandler(this, b.Get(), mMediaIntent.Get());
        // try {
        AutoPtr<IProxy> proxy = (IProxy*)b->Probe(EIID_IProxy);
        assert(proxy != NULL);
        ECode ec = proxy->LinkToDeath((IProxyDeathRecipient*)rcdh.Get(), 0);
        // } catch (RemoteException e) {
            // remote control client is DOA, disqualify it
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::W(TAG, "registerRemoteControlClient() has a dead client %p", b.Get());
            mRcClient = NULL;
        }
        // }
        mRcClientDeathHandler = rcdh;
    }
    return NOERROR;
}

ECode PlayerRecord::ResetControllerInfoForNoRcc()
{
    // stop monitoring the RCC death
    UnlinkToRcClientDeath();
    // reset the RCC-related fields
    mRcClient = NULL;
    mCallingPackageName = NULL;
    return NOERROR;
}

ECode PlayerRecord::Finalize()
{
    Destroy(); // unlink exception handled inside method
    // super.finalize();
    return NOERROR;
}

} // namespace Elastos
} // namespace Droid
} // namespace Media