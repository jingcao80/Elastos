
#include "elastos/droid/server/telecom/Ringer.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/CallAudioManager.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/Log.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Telephony.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/provider/Settings.h>

using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CSystemVibrator;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Telecomm::Telecom::ICallState;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::Utility::CLinkedList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

const AutoPtr<ArrayOf<Int64> > Ringer::VIBRATION_PATTERN = InitVIBRATION_PATTERN();
AutoPtr<IAudioAttributes> Ringer::VIBRATION_ATTRIBUTES = InitVIBRATION_ATTRIBUTES();

const Int32 Ringer::VIBRATION_PATTERN_REPEAT = 1;

Ringer::Ringer()
    : mIsVibrating(FALSE)
{
    CLinkedList::New((IList**)&mRingingCalls);
}

ECode Ringer::constructor(
    /* [in] */ CallAudioManager* callAudioManager,
    /* [in] */ CallsManager* callsManager,
    /* [in] */ InCallTonePlayer::Factory* playerFactory,
    /* [in] */ IContext* context)
{
    mCallAudioManager = IWeakReference::Probe(TO_IINTERFACE(callAudioManager));
    mCallsManager = IWeakReference::Probe(TO_IINTERFACE(callsManager));
    mPlayerFactory = playerFactory;
    mContext = context;
    // We don't rely on getSystemService(IContext::VIBRATOR_SERVICE) to make sure this
    // vibrator object will be isolated from others.
    CSystemVibrator::New(context, (IVibrator**)&mVibrator);
    mRingtonePlayer = new AsyncRingtonePlayer();
    mRingtonePlayer->constructor(context);
    return NOERROR;
}

ECode Ringer::OnCallAdded(
    /* [in] */ ICall* call)
{
    Boolean isIncoming;
    ((Call*) call)->IsIncoming(&isIncoming);
    Int32 callState;
    ((Call*) call)->GetState(&callState);
    if (isIncoming && callState == ICallState::RINGING) {
        Boolean isContains;
        mRingingCalls->Contains(call, &isContains);
        if (isContains) {
            Log::Wtf("Ringer", "New ringing call is already in list of unanswered calls");
        }
        mRingingCalls->Add(call);
        UpdateRinging();
    }
    return NOERROR;
}

ECode Ringer::OnCallRemoved(
    /* [in] */ ICall* call)
{
    return RemoveFromUnansweredCall(call);
}

ECode Ringer::OnCallStateChanged(
    /* [in] */ ICall* call,
    /* [in] */ Int32 oldState,
    /* [in] */ Int32 newState)
{
        if (newState != ICallState::RINGING) {
            RemoveFromUnansweredCall(call);
        }
        return NOERROR;
}

ECode Ringer::OnIncomingCallAnswered(
    /* [in] */ ICall* call)
{
    return OnRespondedToIncomingCall(call);
}

ECode Ringer::OnIncomingCallRejected(
    /* [in] */ ICall* call,
    /* [in] */ Boolean rejectWithMessage,
    /* [in] */ const String& textMessage)
{
    return OnRespondedToIncomingCall(call);
}

ECode Ringer::OnForegroundCallChanged(
    /* [in] */ ICall* oldForegroundCall,
    /* [in] */ ICall* newForegroundCall)
{
    Boolean isContains;
    mRingingCalls->Contains(oldForegroundCall, &isContains);
    Boolean isContainsNew;
    mRingingCalls->Contains(newForegroundCall, &isContainsNew);
    if (isContains || isContainsNew) {
        UpdateRinging();
    }
    return NOERROR;
}

ECode Ringer::Silence()
{
    // Remove all calls from the "ringing" set and then update the ringer.
    mRingingCalls->Clear();
    UpdateRinging();
    return NOERROR;
}

ECode Ringer::OnRespondedToIncomingCall(
    /* [in] */ ICall* call)
{
    // Only stop the ringer if this call is the top-most incoming call.
    AutoPtr<ICall> topMostUnansweredCall;
    GetTopMostUnansweredCall((ICall**)&topMostUnansweredCall);
    if (topMostUnansweredCall.Get() == call) {
        RemoveFromUnansweredCall(call);
    }
    return NOERROR;
}

ECode Ringer::GetTopMostUnansweredCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isEmpty;
    mRingingCalls->IsEmpty(&isEmpty);
    if (isEmpty) {
        *result = NULL;
    }
    else {
        AutoPtr<IInterface> obj;
        mRingingCalls->Get(0, (IInterface**)&obj);
        *result = ICall::Probe(obj);
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode Ringer::RemoveFromUnansweredCall(
    /* [in] */ ICall* call)
{
    mRingingCalls->Remove(call);
    UpdateRinging();
    return NOERROR;
}

ECode Ringer::UpdateRinging()
{
    Boolean isEmpty;
    mRingingCalls->IsEmpty(&isEmpty);
    if (isEmpty) {
        StopRinging();
        StopCallWaiting();
    } else {
        StartRingingOrCallWaiting();
    }
    return NOERROR;
}

ECode Ringer::StartRingingOrCallWaiting()
{
    AutoPtr<ICall> foregroundCall;
    ((CallsManager*) IObject::Probe(mCallsManager))->GetForegroundCall((ICall**)&foregroundCall);
    Log::V("Ringer", "startRingingOrCallWaiting, foregroundCall: %s.", TO_CSTR(foregroundCall));
    Boolean hasActiveOrHoldingCall;
    ((CallsManager*) IObject::Probe(mCallsManager))->HasActiveOrHoldingCall(&hasActiveOrHoldingCall);
    Boolean isContains;
    mRingingCalls->Contains(foregroundCall, &isContains);
    if (isContains && (!hasActiveOrHoldingCall)) {
        // The foreground call is one of incoming calls so play the ringer out loud.
        StopCallWaiting();
        AutoPtr<IUri> contactUri;
        ((Call*) foregroundCall.Get())->GetContactUri((IUri**)&contactUri);
        Boolean shouldRingForContact;
        ShouldRingForContact(contactUri, &shouldRingForContact);
        if (!shouldRingForContact) {
            return NOERROR;
        }
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
        AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(obj);
        Int32 streamVolume;
        audioManager->GetStreamVolume(IAudioManager::STREAM_RING, &streamVolume);
        if (streamVolume >= 0) {
            Log::V("Ringer", "startRingingOrCallWaiting");
            Float startVolume = 0;
            Int32 rampUpTime = 0;
            AutoPtr<IContentResolver> cr;
            mContext->GetContentResolver((IContentResolver**)&cr);
            Int32 i32;
            Settings::System::GetInt32(cr, ISettingsSystem::INCREASING_RING, 0, &i32);
            if (i32 != 0) {
                Settings::System::GetFloat(cr,
                        ISettingsSystem::INCREASING_RING_START_VOLUME, 0.1f, &startVolume);
                Settings::System::GetInt32(cr,
                        ISettingsSystem::INCREASING_RING_RAMP_UP_TIME, 20, &rampUpTime);
            }
            ((CallAudioManager*) IObject::Probe(mCallAudioManager))->SetIsRinging(TRUE);
            // Because we wait until a contact info query to complete before processing a
            // call (for the purposes of direct-to-voicemail), the information about custom
            // ringtones should be available by the time this code executes. We can safely
            // request the custom ringtone from the call and expect it to be current.
            // try {
            ECode ec;
            do {
                AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
                ec = ((Call*) foregroundCall.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
                if (FAILED(ec)) break;
                String phoneAccountHandleId;
                ec = phoneAccountHandle->GetId(&phoneAccountHandleId);
                if (FAILED(ec)) break;
                AutoPtr<ISubscriptionManager> subscriptionManagerHelper;
                CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&subscriptionManagerHelper);
                Int64 i64;
                ec = StringUtils::Parse(phoneAccountHandleId, &i64);
                if (FAILED(ec)) break;
                Int32 phoneId;
                ec = subscriptionManagerHelper->GetPhoneId(i64, &phoneId);
                if (FAILED(ec)) break;
                ec = mRingtonePlayer->SetPhoneId(phoneId);
            } while(FALSE);
            // } catch (NumberFormatException e) {
            if (FAILED(ec)) {
                if ((ECode) E_NUMBER_FORMAT_EXCEPTION == ec) {
                    Log::W("Ringer","Subid is not a number %d", ec);
                    ec = NOERROR;
                }
                else {
                    return ec;
                }
            }
            // }
            AutoPtr<IUri> ringtone;
            ((Call*) foregroundCall.Get())->GetRingtone((IUri**)&ringtone);
            mRingtonePlayer->Play(ringtone, startVolume, rampUpTime);
        } else {
            Log::V("Ringer", "startRingingOrCallWaiting, skipping because volume is 0");
        }
        Boolean shouldVibrate;
        ShouldVibrate(mContext, &shouldVibrate);
        if (shouldVibrate && !mIsVibrating) {
            mVibrator->Vibrate(VIBRATION_PATTERN, VIBRATION_PATTERN_REPEAT,
                    VIBRATION_ATTRIBUTES);
            mIsVibrating = TRUE;
        }
    } else if (foregroundCall != NULL) {
        // The first incoming call added to Telecom is not a foreground call at this point
        // in time. If the current foreground call is null at point, don't play call-waiting
        // as the call will eventually be promoted to the foreground call and play the
        // ring tone.
        Log::V("Ringer", "Playing call-waiting tone.");
        // All incoming calls are in background so play call waiting.
        StopRinging();
        if (mCallWaitingPlayer == NULL) {
            mPlayerFactory->CreatePlayer(InCallTonePlayer::TONE_CALL_WAITING, (InCallTonePlayer**)&mCallWaitingPlayer);
            mCallWaitingPlayer->StartTone();
        }
    }
    return NOERROR;
}

ECode Ringer::ShouldRingForContact(
    /* [in] */ IUri* contactUri,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
    AutoPtr<INotificationManager> manager = INotificationManager::Probe(obj);
    AutoPtr<IBundle> extras;
    CBundle::New((IBundle**)&extras);
    if (contactUri != NULL) {
        AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
        (*array)[0] = TO_STR(contactUri);
        extras->PutStringArray(INotification::EXTRA_PEOPLE, array);
    }
    assert(0 && "TODO INotificationManager");
    return NOERROR;
    // return manager->MatchesCallFilter(extras, result);
}

ECode Ringer::StopRinging()
{
    Log::V("Ringer", "stopRinging");
    mRingtonePlayer->Stop();
    if (mIsVibrating) {
        mVibrator->Cancel();
        mIsVibrating = FALSE;
    }
    // Even though stop is asynchronous it's ok to update the audio manager. Things like audio
    // focus are voluntary so releasing focus too early is not detrimental.
    ((CallAudioManager*) IObject::Probe(mCallAudioManager))->SetIsRinging(FALSE);
    return NOERROR;
}

ECode Ringer::StopCallWaiting()
{
    Log::V("Ringer", "stop call waiting.");
    if (mCallWaitingPlayer != NULL) {
        mCallWaitingPlayer->StopTone();
        mCallWaitingPlayer = NULL;
    }
    return NOERROR;
}

ECode Ringer::ShouldVibrate(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(obj);
    Int32 ringerMode;
    audioManager->GetRingerMode(&ringerMode);
    Boolean isVibrateWhenRinging;
    GetVibrateWhenRinging(context, &isVibrateWhenRinging);
    if (isVibrateWhenRinging) {
        *result = ringerMode != IAudioManager::RINGER_MODE_SILENT;
    } else {
        *result = ringerMode == IAudioManager::RINGER_MODE_VIBRATE;
    }
    return NOERROR;
}

ECode Ringer::GetVibrateWhenRinging(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean hasVibrator;
    mVibrator->HasVibrator(&hasVibrator);
    if (!hasVibrator) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    Int32 vibrateWhenRinging;
    Settings::System::GetInt32(contentResolver,
            ISettingsSystem::VIBRATE_WHEN_RINGING, 0, &vibrateWhenRinging);
    *result = vibrateWhenRinging != 0;
    return NOERROR;
}

AutoPtr<ArrayOf<Int64> > Ringer::InitVIBRATION_PATTERN()
{
    AutoPtr<ArrayOf<Int64> > rev = ArrayOf<Int64>::Alloc(3);
    (*rev)[0] = 0;
    (*rev)[1] = 1000;
    (*rev)[2] = 1000;
    return rev;
}

AutoPtr<IAudioAttributes> Ringer::InitVIBRATION_ATTRIBUTES()
{
    AutoPtr<IAudioAttributes> rev;
    AutoPtr<IAudioAttributesBuilder> builder;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&builder);
    builder->SetContentType(IAudioAttributes::CONTENT_TYPE_SONIFICATION);
    builder->SetUsage(IAudioAttributes::USAGE_NOTIFICATION_RINGTONE);
    builder->Build((IAudioAttributes**)&rev);
    return rev;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
