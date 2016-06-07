
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CALL_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CALL_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/ConnectionServiceRepository.h"
#include "elastos/droid/server/telecom/CreateConnectionProcessor.h"
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.Telecomm.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Internal::Telephony::ICallerInfo;
using Elastos::Droid::Internal::Telephony::ICallerInfoAsyncQueryOnQueryCompleteListener;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Telecomm::Internal::IIVideoProvider;
using Elastos::Droid::Telecomm::Telecom::IDisconnectCause;
using Elastos::Droid::Telecomm::Telecom::IGatewayInfo;
using Elastos::Droid::Telecomm::Telecom::IResponse;
using Elastos::Droid::Telecomm::Telecom::IStatusHints;
using Elastos::Core::IRunnable;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 *  Encapsulates all aspects of a given phone call throughout its lifecycle, starting
 *  from the time the call intent was received by Telecom (vs. the time the call was
 *  connected etc).
 */
class Call
    : public Object
    , public ICreateConnectionResponse
    , public ICall
{
    friend class SubRunnable;

public:
    class ListenerBase
        : public Object
        , public ICallListener
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnSuccessfulOutgoingCall(
            /* [in] */ ICall* call,
            /* [in] */ Int32 callState);

        // @Override
        CARAPI OnFailedOutgoingCall(
            /* [in] */ ICall* call,
            /* [in] */ IDisconnectCause* disconnectCause);

        // @Override
        CARAPI OnSuccessfulIncomingCall(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnFailedIncomingCall(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnSuccessfulUnknownCall(
            /* [in] */ ICall* call,
            /* [in] */ Int32 callState);

        // @Override
        CARAPI OnFailedUnknownCall(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnRingbackRequested(
            /* [in] */ ICall* call,
            /* [in] */ Boolean ringbackRequested);

        // @Override
        CARAPI OnPostDialWait(
            /* [in] */ ICall* call,
            /* [in] */ const String& remaining);

        // @Override
        CARAPI OnCallCapabilitiesChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnCallPropertiesChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnParentChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnChildrenChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnCannedSmsResponsesLoaded(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnVideoCallProviderChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnCallerInfoChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnIsVoipAudioModeChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnStatusHintsChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnHandleChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnCallerDisplayNameChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnVideoStateChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnTargetPhoneAccountChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnConnectionManagerPhoneAccountChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnPhoneAccountChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnConferenceableCallsChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnCallSubstateChanged(
            /* [in] */ ICall* call);
    };

    class SubCallerInfoAsyncQueryOnQueryCompleteListener
        : public Object
        , public ICallerInfoAsyncQueryOnQueryCompleteListener
    {
    public:
        CAR_INTERFACE_DECL()

        /** ${inheritDoc} */
        // @Override
        CARAPI OnQueryComplete(
            /* [in] */ Int32 token,
            /* [in] */ IInterface* cookie,
            /* [in] */ ICallerInfo* callerInfo);
    };

    class SubContactsAsyncHelperOnImageLoadCompleteListener
        : public Object
        , public IContactsAsyncHelperOnImageLoadCompleteListener
    {
    public:
        CAR_INTERFACE_DECL()

        /** ${inheritDoc} */
        // @Override
        CARAPI OnImageLoadComplete(
            /* [in] */ Int32 token,
            /* [in] */ IDrawable* photo,
            /* [in] */ IBitmap* photoIcon,
            /* [in] */ IObject* cookie);
    };

    class SubRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        SubRunnable(
            /* [in] */ Call* host);

        // @Override
        CARAPI Run();

    private:
        Call* mHost;
    };

private:
    class SubResponse
        : public Object
        , public IResponse
    {
    public:
        CAR_INTERFACE_DECL()

        SubResponse(
            /* [in] */ Call* host);

        // @Override
        CARAPI OnResult(
            /* [in] */ IInterface* request,
            /* [in] */ ArrayOf<IInterface*>* result);

        // @Override
        CARAPI OnError(
            /* [in] */ IInterface* request,
            /* [in] */ Int32 code,
            /* [in] */ const String& msg);

    private:
        Call* mHost;
    };


public:
    CAR_INTERFACE_DECL()

    Call();

    /**
     * Persists the specified parameters and initializes the new instance.
     *
     * @param context The context.
     * @param repository The connection service repository.
     * @param handle The handle to dial.
     * @param gatewayInfo Gateway information to use for the call.
     * @param connectionManagerPhoneAccountHandle Account to use for the service managing the call.
     *         This account must be one that was registered with the
     *         {@link PhoneAccount#CAPABILITY_CONNECTION_MANAGER} flag.
     * @param targetPhoneAccountHandle Account information to use for the call. This account must be
     *         one that was registered with the {@link PhoneAccount#CAPABILITY_CALL_PROVIDER} flag.
     * @param isIncoming True if this is an incoming call.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ConnectionServiceRepository* repository,
        /* [in] */ IUri* handle,
        /* [in] */ IGatewayInfo* gatewayInfo,
        /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccountHandle,
        /* [in] */ IPhoneAccountHandle* targetPhoneAccountHandle,
        /* [in] */ Boolean isIncoming,
        /* [in] */ Boolean isConference);

    CARAPI AddListener(
        /* [in] */ ICallListener* listener);

    CARAPI RemoveListener(
        /* [in] */ ICallListener* listener);

    /** {@inheritDoc} */
    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetState(
        /* [out] */ Int32* result);

    /**
     * Sets the call state. Although there exists the notion of appropriate state transitions
     * (see {@link CallState}), in practice those expectations break down when cellular systems
     * misbehave and they do this very often. The result is that we do not enforce state transitions
     * and instead keep the code resilient to unexpected state changes.
     */
    CARAPI SetState(
        /* [in] */ Int32 newState);

    CARAPI SetRingbackRequested(
        /* [in] */ Boolean ringbackRequested);

    CARAPI IsRingbackRequested(
        /* [out] */ Boolean* result);

    CARAPI IsConference(
        /* [out] */ Boolean* result);

    CARAPI GetHandle(
        /* [out] */ IUri** result);

    CARAPI GetHandlePresentation(
        /* [out] */ Int32* result);

    CARAPI SetHandle(
        /* [in] */ IUri* handle);

    CARAPI SetHandle(
        /* [in] */ IUri* handle,
        /* [in] */ Int32 presentation);

    CARAPI GetCallerDisplayName(
        /* [out] */ String* result);

    CARAPI GetCallerDisplayNamePresentation(
        /* [out] */ Int32* result);

    CARAPI SetCallerDisplayName(
        /* [in] */ const String& callerDisplayName,
        /* [in] */ Int32 presentation);

    CARAPI GetName(
        /* [out] */ String* result);

    CARAPI GetNumber(
        /* [out] */ String* result);

    CARAPI GetPhotoIcon(
        /* [out] */ IBitmap** result);

    CARAPI GetPhoto(
        /* [out] */ IDrawable** result);

    /**
     * @param disconnectCause The reason for the disconnection, represented by
     *         {@link android.telecom.DisconnectCause}.
     */
    CARAPI SetDisconnectCause(
        /* [in] */ IDisconnectCause* disconnectCause);

    CARAPI GetDisconnectCause(
        /* [out] */ IDisconnectCause** result);

    CARAPI IsEmergencyCall(
        /* [out] */ Boolean* result);

    /**
     * @return The original handle this call is associated with. In-call services should use this
     * handle when indicating in their UI the handle that is being called.
     */
    CARAPI GetOriginalHandle(
        /* [out] */ IUri** result);

    CARAPI GetGatewayInfo(
        /* [out] */ IGatewayInfo** result);

    CARAPI SetGatewayInfo(
        /* [in] */ IGatewayInfo* gatewayInfo);

    CARAPI GetConnectionManagerPhoneAccount(
        /* [out] */ IPhoneAccountHandle** result);

    CARAPI SetConnectionManagerPhoneAccount(
        /* [in] */ IPhoneAccountHandle* accountHandle);

    CARAPI GetTargetPhoneAccount(
        /* [out] */ IPhoneAccountHandle** result);

    CARAPI SetTargetPhoneAccount(
        /* [in] */ IPhoneAccountHandle* accountHandle);

    CARAPI IsIncoming(
        /* [out] */ Boolean* result);

    /**
     * @return The "age" of this call object in milliseconds, which typically also represents the
     *     period since this call was added to the set pending outgoing calls, see
     *     mCreationTimeMillis.
     */
    CARAPI GetAgeMillis(
        /* [out] */ Int64* result);

    /**
     * @return the duration of the call.
     */
    CARAPI GetDurationMillis(
        /* [out] */ Int64* result);

    /**
     * @return The time when this call object was created and added to the set of pending outgoing
     *     calls.
     */
    CARAPI GetCreationTimeMillis(
        /* [out] */ Int64* result);

    CARAPI SetCreationTimeMillis(
        /* [in] */ Int64 time);

    CARAPI GetConnectTimeMillis(
        /* [out] */ Int64* result);

    CARAPI SetConnectTimeMillis(
        /* [in] */ Int64 connectTimeMillis);

    CARAPI GetCallCapabilities(
        /* [out] */ Int32* result);

    CARAPI SetCallCapabilities(
        /* [in] */ Int32 callCapabilities);

    CARAPI SetCallCapabilities(
        /* [in] */ Int32 callCapabilities,
        /* [in] */ Boolean forceUpdate);

    CARAPI GetCallProperties(
        /* [out] */ Int32* result);

    CARAPI SetCallProperties(
        /* [in] */ Int32 callProperties);

    CARAPI GetParentCall(
        /* [out] */ ICall** result);

    CARAPI GetChildCalls(
        /* [out] */ IList** result);

    CARAPI WasConferencePreviouslyMerged(
        /* [out] */ Boolean* result);

    CARAPI GetConferenceLevelActiveCall(
        /* [out] */ ICall** result);

    CARAPI GetConnectionService(
        /* [out] */ IConnectionServiceWrapper** result);

    /**
     * Retrieves the {@link Context} for the call.
     *
     * @return The {@link Context}.
     */
    CARAPI GetContext(
        /* [out] */ IContext** result);

    CARAPI SetConnectionService(
        /* [in] */ IConnectionServiceWrapper* service);

    /**
     * Clears the associated connection service.
     */
    CARAPI ClearConnectionService();

    /**
     * Starts the create connection sequence. Upon completion, there should exist an active
     * connection through a connection service (or the call will have failed).
     *
     * @param phoneAccountRegistrar The phone account registrar.
     */
    CARAPI StartCreateConnection(
        /* [in] */ PhoneAccountRegistrar* phoneAccountRegistrar);

    // @Override
    CARAPI HandleCreateConnectionSuccess(
        /* [in] */ ICallIdMapper* idMapper,
        /* [in] */ IParcelableConnection* connection);

    // @Override
    CARAPI HandleCreateConnectionFailure(
        /* [in] */ IDisconnectCause* disconnectCause);

    /**
     * Plays the specified DTMF tone.
     */
    CARAPI PlayDtmfTone(
        /* [in] */ Char32 digit);

    /**
     * Stops playing any currently playing DTMF tone.
     */
    CARAPI StopDtmfTone();

    /**
     * Attempts to disconnect the call through the connection service.
     */
    CARAPI Disconnect();

    CARAPI Abort();

    /**
     * Answers the call if it is ringing.
     *
     * @param videoState The video state in which to answer the call.
     */
    CARAPI Answer(
        /* [in] */ Int32 videoState);

    /**
     * Deflects the call if it is ringing.
     *
     * @param number Number to be deflected to.
     */
    CARAPI Deflect(
        /* [in] */ const String& number);

    /**
     * Rejects the call if it is ringing.
     *
     * @param rejectWithMessage Whether to send a text message as part of the call rejection.
     * @param textMessage An optional text message to send as part of the rejection.
     */
    CARAPI Reject(
        /* [in] */ Boolean rejectWithMessage,
        /* [in] */ const String& textMessage);

    /**
     * Puts the call on hold if it is currently active.
     */
    CARAPI Hold();

    /**
     * Releases the call from hold if it is currently active.
     */
    CARAPI Unhold();

    CARAPI SetLocalCallHold(
        /* [in] */ Int32 lchState);

    CARAPI SetActiveSubscription();

    /** Checks if this is a live call or not. */
    CARAPI IsAlive(
        /* [out] */ Boolean* result);

    CARAPI IsActive(
        /* [out] */ Boolean* result);

    CARAPI GetExtras(
        /* [out] */ IBundle** result);

    CARAPI SetExtras(
        /* [in] */ IBundle* extras);

    /**
     * @return the uri of the contact associated with this call.
     */
    CARAPI GetContactUri(
        /* [out] */ IUri** result);

    CARAPI GetRingtone(
        /* [out] */ IUri** result);

    CARAPI OnPostDialWait(
        /* [in] */ const String& remaining);

    CARAPI PostDialContinue(
        /* [in] */ Boolean proceed);

    CARAPI ConferenceWith(
        /* [in] */ ICall* otherCall);

    CARAPI SplitFromConference();

    CARAPI MergeConference();

    CARAPI SwapConference();

    CARAPI SetParentCall(
        /* [in] */ ICall* parentCall);

    CARAPI SetConferenceableCalls(
        /* [in] */ IList* conferenceableCalls);

    CARAPI GetConferenceableCalls(
        /* [out] */ IList** result);

    CARAPI Can(
        /* [in] */ Int32 capability,
        /* [out] */ Boolean* result);

    /**
     * Return whether the user can respond to this {@code Call} via an SMS message.
     *
     * @return true if the "Respond via SMS" feature should be enabled
     * for this incoming call.
     *
     * The general rule is that we *do* allow "Respond via SMS" except for
     * the few (relatively rare) cases where we know for sure it won't
     * work, namely:
     *   - a bogus or blank incoming number
     *   - a call from a SIP address
     *   - a "call presentation" that doesn't allow the number to be revealed
     *
     * In all other cases, we allow the user to respond via SMS.
     *
     * Note that this behavior isn't perfect; for example we have no way
     * to detect whether the incoming call is from a landline (with most
     * networks at least), so we still enable this feature even though
     * SMSes to that number will silently fail.
     */
    CARAPI IsRespondViaSmsCapable(
        /* [out] */ Boolean* result);

    CARAPI GetCannedSmsResponses(
        /* [out] */ IList** result);

    CARAPI GetCallerInfo(
        /* [out] */ ICallerInfo** result);

    /**
     * Sets speakerphone option on when call begins.
     */
    CARAPI SetStartWithSpeakerphoneOn(
        /* [in] */ Boolean startWithSpeakerphone);

    /**
     * Returns speakerphone option.
     *
     * @return Whether or not speakerphone should be set automatically when call begins.
     */
    CARAPI GetStartWithSpeakerphoneOn(
        /* [out] */ Boolean* result);

    /**
     * Sets a video call provider for the call.
     */
    CARAPI SetVideoProvider(
        /* [in] */ IIVideoProvider* videoProvider);

    /**
     * @return Return the {@link Connection.VideoProvider} binder.
     */
    CARAPI GetVideoProvider(
        /* [out] */ IIVideoProvider** result);

    /**
     * The current video state for the call.
     * Valid values: see {@link VideoProfile.VideoState}.
     */
    CARAPI GetVideoState(
        /* [out] */ Int32* result);

    /**
     * Returns the video states which were applicable over the duration of a call.
     * See {@link VideoProfile} for a list of valid video states.
     *
     * @return The video states applicable over the duration of the call.
     */
    CARAPI GetVideoStateHistory(
        /* [out] */ Int32* result);

    /**
     * Determines the current video state for the call.
     * For an outgoing call determines the desired video state for the call.
     * Valid values: see {@link VideoProfile.VideoState}
     *
     * @param videoState The video state for the call.
     */
    CARAPI SetVideoState(
        /* [in] */ Int32 videoState);

    CARAPI GetIsVoipAudioMode(
        /* [out] */ Boolean* result);

    CARAPI SetIsVoipAudioMode(
        /* [in] */ Boolean audioModeIsVoip);

    CARAPI GetStatusHints(
        /* [out] */ IStatusHints** result);

    CARAPI SetStatusHints(
        /* [in] */ IStatusHints* statusHints);

    CARAPI IsUnknown(
        /* [out] */ Boolean* result);

    CARAPI SetIsUnknown(
        /* [in] */ Boolean isUnknown);

    /**
     * Determines if this call is in a disconnecting state.
     *
     * @return {@code true} if this call is locally disconnecting.
     */
    CARAPI IsLocallyDisconnecting(
        /* [out] */ Boolean* result);

    static CARAPI GetStateFromConnectionState(
        /* [in] */ Int32 state,
        /* [out] */ Int32* result);

    /**
     * The current call substate.
     */
    CARAPI GetCallSubstate(
        /* [out] */ Int32* result);

    /**
     * Determines the current substate for the call.
     *
     * @param callSubstate The substate for the call.
     */
    CARAPI SetCallSubstate(
        /* [in] */ Int32 callSubstate);

private:
    CARAPI ProcessDirectToVoicemail();

    CARAPI AddChildCall(
        /* [in] */ ICall* call);

    CARAPI RemoveChildCall(
        /* [in] */ ICall* call);

    /**
     * We need to make sure that before we move a call to the disconnected state, it no
     * longer has any parent/child relationships.  We want to do this to ensure that the InCall
     * Service always has the right data in the right order.  We also want to do it in telecom so
     * that the insurance policy lives in the framework side of things.
     */
    CARAPI FixParentAfterDisconnect();

    /**
     * @return True if the call is ringing, else logs the action name.
     */
    CARAPI IsRinging(
        /* [in] */ const String& actionName,
        /* [out] */ Boolean* result);

    // @SuppressWarnings("rawtypes")
    CARAPI DecrementAssociatedCallCount(
        /* [in] */ IServiceBinder* binder);

    /**
     * Looks up contact information based on the current handle.
     */
    CARAPI StartCallerInfoLookup();

    /**
     * Saves the specified caller info if the specified token matches that of the last query
     * that was made.
     *
     * @param callerInfo The new caller information to set.
     * @param token The token used with this query.
     */
    CARAPI SetCallerInfo(
        /* [in] */ ICallerInfo* callerInfo,
        /* [in] */ Int32 token);

    /**
     * Saves the specified photo information if the specified token matches that of the last query.
     *
     * @param photo The photo as a drawable.
     * @param photoIcon The photo as a small icon.
     * @param token The token used with this query.
     */
    CARAPI SetPhoto(
        /* [in] */ IDrawable* photo,
        /* [in] */ IBitmap* photoIcon,
        /* [in] */ Int32 token);

    CARAPI MaybeLoadCannedSmsResponses();

    /**
     * Sets whether this call is in a disconnecting state.
     *
     * @param isLocallyDisconnecting {@code true} if this call is locally disconnecting.
     */
    CARAPI SetLocallyDisconnecting(
        /* [in] */ Boolean isLocallyDisconnecting);

public:
    Boolean mIsActiveSub;

    // Key used to pack OEM call extras within a Call object's
    // mExtras Bundle. Used by the setCallExtras method in
    // CallsManager.
    static const String KEY_OEM_EXTRAS;

private:
    static AutoPtr<ICallerInfoAsyncQueryOnQueryCompleteListener> sCallerInfoQueryListener;

    static AutoPtr<IContactsAsyncHelperOnImageLoadCompleteListener> sPhotoLoadListener;

    AutoPtr<IRunnable> mDirectToVoicemailRunnable;

    /** True if this is an incoming call. */
    Boolean mIsIncoming;

    /** True if this is a currently unknown call that was not previously tracked by CallsManager,
     *  and did not originate via the regular incoming/outgoing call code paths.
     */
    Boolean mIsUnknown;

    /**
     * The time this call was created. Beyond logging and such, may also be used for bookkeeping
     * and specifically for marking certain call attempts as failed attempts.
     */
    Int64 mCreationTimeMillis;

    /** The gateway information associated with this call. This stores the original call handle
     * that the user is attempting to connect to via the gateway, the actual handle to dial in
     * order to connect the call via the gateway, as well as the package name of the gateway
     * service. */
    AutoPtr<IGatewayInfo> mGatewayInfo;

    AutoPtr<IPhoneAccountHandle> mConnectionManagerPhoneAccountHandle;

    AutoPtr<IPhoneAccountHandle> mTargetPhoneAccountHandle;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IList> mConferenceableCalls;

    Int64 mConnectTimeMillis;

    /** The state of the call. */
    Int32 mState;

    /** The handle with which to establish this call. */
    AutoPtr<IUri> mHandle;

    /**
     * The presentation requirements for the handle. See {@link TelecomManager} for valid values.
     */
    Int32 mHandlePresentation;

    /** The caller display name (CNAP) set by the connection service. */
    String mCallerDisplayName;

    /**
     * The presentation requirements for the handle. See {@link TelecomManager} for valid values.
     */
    Int32 mCallerDisplayNamePresentation;

    /**
     * The connection service which is attempted or already connecting this call.
     */
    AutoPtr<IConnectionServiceWrapper> mConnectionService;

    Boolean mIsEmergencyCall;

    Boolean mSpeakerphoneOn;

    /**
     * Tracks the video states which were applicable over the duration of a call.
     * See {@link VideoProfile} for a list of valid video states.
     */
    Int32 mVideoStateHistory;

    Int32 mVideoState;

    /**
     * Disconnect cause for the call. Only valid if the state of the call is STATE_DISCONNECTED.
     * See {@link android.telecom.DisconnectCause}.
     */
    AutoPtr<IDisconnectCause> mDisconnectCause;

    /** Info used by the connection services. */
    AutoPtr<IBundle> mExtras;

    /** Set of listeners on this call.
     *
     * ConcurrentHashMap constructor params: 8 is initial table size, 0.9f is
     * load factor before resizing, 1 means we only expect a single thread to
     * access the map so make only a single shard
     */
    AutoPtr<ISet> mListeners;

    AutoPtr<CreateConnectionProcessor> mCreateConnectionProcessor;

    /** Caller information retrieved from the latest contact query. */
    AutoPtr<ICallerInfo> mCallerInfo;

    /** The latest token used with a contact info query. */
    Int32 mQueryToken;

    /** Whether this call is requesting that Telecom play the ringback tone on its behalf. */
    Boolean mRingbackRequested;

    /** Whether direct-to-voicemail query is pending. */
    Boolean mDirectToVoicemailQueryPending;

    Int32 mCallCapabilities;

    Int32 mCallProperties;

    Boolean mIsConference;

    AutoPtr<ICall> mParentCall;

    AutoPtr<IList> mChildCalls;

    /** Set of text message responses allowed for this call, if applicable. */
    AutoPtr<IList> mCannedSmsResponses;

    /** Whether an attempt has been made to load the text message responses. */
    Boolean mCannedSmsResponsesLoadingStarted;

    AutoPtr<IIVideoProvider> mVideoProvider;

    Boolean mIsVoipAudioMode;

    AutoPtr<IStatusHints> mStatusHints;

    AutoPtr<ConnectionServiceRepository> mRepository;

    AutoPtr<IContext> mContext;

    Int32 mCallSubstate;

    Boolean mWasConferencePreviouslyMerged;

    // switches every time the user hits "swap".
    AutoPtr<ICall> mConferenceLevelActiveCall;

    Boolean mIsLocallyDisconnecting;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CALL_H__
