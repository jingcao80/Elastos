
#include "elastos/droid/internal/telephony/Connection.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.Droid.Telecomm.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::SystemClock;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Concurrent::CCopyOnWriteArraySet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const Int32 Connection::AUDIO_QUALITY_STANDARD = 1;
const Int32 Connection::AUDIO_QUALITY_HIGH_DEFINITION = 2;
String Connection::TAG("Connection");

CAR_INTERFACE_IMPL(Connection::ListenerBase, Object, IConnectionListener)
Connection::ListenerBase::~ListenerBase()
{}

ECode Connection::ListenerBase::OnVideoStateChanged(
    /* [in] */ Int32 videoState)
{
    return NOERROR;
}

ECode Connection::ListenerBase::OnLocalVideoCapabilityChanged(
    /* [in] */ Boolean capable)
{
    return NOERROR;
}

ECode Connection::ListenerBase::OnRemoteVideoCapabilityChanged(
    /* [in] */ Boolean capable)
{
    return NOERROR;
}

ECode Connection::ListenerBase::OnVideoProviderChanged(
    /* [in] */ IConnectionVideoProvider* videoProvider)
{
    return NOERROR;
}

ECode Connection::ListenerBase::OnAudioQualityChanged(
    /* [in] */ Int32 audioQuality)
{
    return NOERROR;
}

ECode Connection::ListenerBase::OnCallSubstateChanged(
    /* [in] */ Int32 callSubstate)
{
    return NOERROR;
}

ECode Connection::ListenerBase::OnConferenceParticipantsChanged(
    /* [in] */ IList* participants)
{
    return NOERROR;
}

CAR_INTERFACE_IMPL(Connection, Object, IConnection)
Connection::Connection()
    : mPreHandoverState(ICallState_IDLE)
    , mCnapNamePresentation(IPhoneConstants::PRESENTATION_ALLOWED)
    , mNumberPresentation(IPhoneConstants::PRESENTATION_ALLOWED)
    , mIsIncoming(FALSE)
    , mCreateTime(0)
    , mConnectTime(0)
    , mConnectTimeReal(0)
    , mDuration(0)
    , mHoldingStartTime(0)
    , mNumberConverted(FALSE)
    , mVideoState(0)
    , mLocalVideoCapable(FALSE)
    , mRemoteVideoCapable(FALSE)
    , mAudioQuality(0)
    , mCallSubstate(0)
{
    CCopyOnWriteArraySet::New((ISet**)&mListeners);
    CArrayList::New((IList**)&mPostDialListeners);
}

Connection::~Connection()
{}

/* Instance Methods */
/**
 * Gets address (e.g. phone number) associated with connection.
 * TODO: distinguish reasons for unavailability
 *
 * @return address or NULL if unavailable
 */
ECode Connection::GetAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAddress;
    return NOERROR;
}

/**
 * Gets CNAP name associated with connection.
 * @return cnap name or NULL if unavailable
 */
ECode Connection::GetCnapName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCnapName;
    return NOERROR;
}

/**
 * Get original dial string.
 * @return original dial string or NULL if unavailable
 */
ECode Connection::GetOrigDialString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

/**
 * Gets CNAP presentation associated with connection.
 * @return cnap name or NULL if unavailable
 */

ECode Connection::GetCnapNamePresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCnapNamePresentation;
    return NOERROR;
}

/**
 * Connection create time in currentTimeMillis() format
 * Basically, set when object is created.
 * Effectively, when an incoming call starts ringing or an
 * outgoing call starts dialing
 */
ECode Connection::GetCreateTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCreateTime;
    return NOERROR;
}

/**
 * Connection connect time in currentTimeMillis() format.
 * For outgoing calls: Begins at (DIALING|ALERTING) -> ACTIVE transition.
 * For incoming calls: Begins at (INCOMING|WAITING) -> ACTIVE transition.
 * Returns 0 before then.
 */
ECode Connection::GetConnectTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mConnectTime;
    return NOERROR;
}

/**
 * Connection connect time in elapsedRealtime() format.
 * For outgoing calls: Begins at (DIALING|ALERTING) -> ACTIVE transition.
 * For incoming calls: Begins at (INCOMING|WAITING) -> ACTIVE transition.
 * Returns 0 before then.
 */
ECode Connection::GetConnectTimeReal(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mConnectTimeReal;
    return NOERROR;
}

/**
 * Returns the number of milliseconds the call has been connected,
 * or 0 if the call has never connected.
 * If the call is still connected, then returns the elapsed
 * time since connect.
 */
ECode Connection::GetDurationMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    if (mConnectTimeReal == 0) {
        *result = 0;
        return NOERROR;
    }
    else if (mDuration == 0) {
        *result = SystemClock::GetElapsedRealtime() - mConnectTimeReal;
        return NOERROR;
    }

    *result = mDuration;
    return NOERROR;
}

/**
 * The time when this Connection last transitioned into HOLDING
 * in elapsedRealtime() format.
 * Returns 0, if it has never made a transition into HOLDING.
 */
ECode Connection::GetHoldingStartTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHoldingStartTime;
    return NOERROR;
}

/**
 * Returns TRUE of this connection originated elsewhere
 * ("MT" or mobile terminated; another party called this terminal)
 * or FALSE if this call originated here (MO or mobile originated).
 */
ECode Connection::IsIncoming(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsIncoming;
    return NOERROR;
}

/**
 * If this Connection is connected, then it is associated with
 * a Call.
 *
 * Returns getCall().getState() or Call.State.IDLE if not
 * connected
 */
ECode Connection::GetState(
    /* [out] */ ICallState* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICall> c;

    GetCall((ICall**)&c);

    if (c == NULL) {
        *result = ICallState_IDLE;
    }

    return c->GetState(result);
}

/**
 * If this connection went through handover return the state of the
 * call that contained this connection before handover.
 */
ECode Connection::GetStateBeforeHandover(
    /* [out] */ ICallState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPreHandoverState;
    return NOERROR;
}

/**
 * Get the extras for the connection's call.
 *
 * Returns getCall().getExtras()
 */
ECode Connection::GetExtras(
    /* [out] */ IBundle** extras)
{
    AutoPtr<ICall> c;

    GetCall((ICall**)&c);

    if (c == NULL) {
        *extras = NULL;
        return NOERROR;
    }

    return c->GetExtras(extras);
}

/**
 * isAlive()
 *
 * @return TRUE if the connection isn't disconnected
 * (could be active, holding, ringing, dialing, etc)
 */
ECode Connection::IsAlive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    ICallState cs;
    GetState(&cs);
    *result = !(cs == ICallState_IDLE || cs == ICallState_DISCONNECTED || cs == ICallState_DISCONNECTING);
    return NOERROR;
}

/**
 * Returns TRUE if Connection is connected and is INCOMING or WAITING
 */
ECode Connection::IsRinging(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    ICallState cs;
    GetState(&cs);
    *result = cs == ICallState_INCOMING || cs == ICallState_WAITING;
    return NOERROR;
}

/**
 *
 * @return the userdata set in setUserData()
 */
ECode Connection::GetUserData(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUserData;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 *
 * @param userdata user can store an any userdata in the Connection object.
 */
ECode Connection::SetUserData(
    /* [in] */ IInterface* userdata)
{
    mUserData = userdata;
    return NOERROR;
}

ECode Connection::ClearUserData()
{
    mUserData = NULL;
    return NOERROR;
}

ECode Connection::AddPostDialListener(
    /* [in] */ IConnectionPostDialListener* listener)
{
    Boolean tmp = FALSE;
    if (mPostDialListeners->Contains(listener, &tmp), !tmp) {
        mPostDialListeners->Add(listener);
    }
    return NOERROR;
}

void Connection::ClearPostDialListeners()
{
    mPostDialListeners->Clear();
}

void Connection::NotifyPostDialListeners()
{
    IConnectionPostDialState ps;
    if ((GetPostDialState(&ps), ps) == PostDialState_WAIT) {
        AutoPtr<IIterator> ator;
        mPostDialListeners->GetIterator((IIterator**)&ator);
        Boolean hasNext = FALSE;
        while (ator->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            ator->GetNext((IInterface**)&obj);
            AutoPtr<IConnectionPostDialListener> listener = IConnectionPostDialListener::Probe(obj);
            listener->OnPostDialWait();
        }
    }
}

/**
 * Returns the original Connection instance associated with
 * this Connection
 */
ECode Connection::GetOrigConnection(
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOrigConnection;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Connection::MigrateFrom(
    /* [in] */ IConnection* _c)
{
    if (_c == NULL) return NOERROR;
    Connection* c = (Connection*)_c;
    mListeners = c->mListeners;
    c->GetAddress(&mAddress);
    c->GetNumberPresentation(&mNumberPresentation);
    c->GetOrigDialString(&mDialString);
    c->GetCnapName(&mCnapName);
    c->GetCnapNamePresentation(&mCnapNamePresentation);
    c->IsIncoming(&mIsIncoming);
    c->GetCreateTime(&mCreateTime);
    c->GetConnectTime(&mConnectTime);
    c->GetConnectTimeReal(&mConnectTimeReal);
    c->GetHoldingStartTime(&mHoldingStartTime);
    c->GetOrigConnection((IConnection**)&mOrigConnection);
    return NOERROR;
}

/**
 * Assign a listener to be notified of state changes.
 *
 * @param listener A listener.
 */
ECode Connection::AddListener(
    /* [in] */ IConnectionListener* listener)
{
    mListeners->Add(listener);
    return NOERROR;
}

/**
 * Removes a listener.
 *
 * @param listener A listener.
 */
ECode Connection::RemoveListener(
    /* [in] */ IConnectionListener* listener)
{
    mListeners->Remove(listener);
    return NOERROR;
}

/**
 * Returns the current video state of the connection.
 *
 * @return The video state of the connection.
 */
ECode Connection::GetVideoState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVideoState;
    return NOERROR;
}

/**
 * Returns the local video capability state for the connection.
 *
 * @return {@code True} if the connection has local video capabilities.
 */
ECode Connection::IsLocalVideoCapable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLocalVideoCapable;
    return NOERROR;
}

/**
 * Returns the remote video capability state for the connection.
 *
 * @return {@code True} if the connection has remote video capabilities.
 */
ECode Connection::IsRemoteVideoCapable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRemoteVideoCapable;
    return NOERROR;
}

/**
 * Returns the {@link android.telecom.Connection.VideoProvider} for the connection.
 *
 * @return The {@link android.telecom.Connection.VideoProvider}.
 */
ECode Connection::GetVideoProvider(
    /* [out] */ IConnectionVideoProvider** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVideoProvider;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Returns the audio-quality for the connection.
 *
 * @return The audio quality for the connection.
 */
ECode Connection::GetAudioQuality(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAudioQuality;
    return NOERROR;
}

/**
 * Returns the current call substate of the connection.
 *
 * @return The call substate of the connection.
 */
ECode Connection::GetCallSubstate(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCallSubstate;
    return NOERROR;
}

/**
 * Sets the videoState for the current connection and reports the changes to all listeners.
 * Valid video states are defined in {@link android.telecom.VideoProfile}.
 *
 * @return The video state.
 */
ECode Connection::SetVideoState(
    /* [in] */ Int32 videoState)
{
    mVideoState = videoState;

    AutoPtr<IIterator> ator;
    mListeners->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IConnectionListener> l = IConnectionListener::Probe(obj);
        l->OnVideoStateChanged(mVideoState);
    }
    return NOERROR;
}

/**
 * Sets whether video capability is present locally.
 *
 * @param capable {@code True} if video capable.
 */
ECode Connection::SetLocalVideoCapable(
    /* [in] */ Boolean capable)
{
    mLocalVideoCapable = capable;
    AutoPtr<IIterator> ator;
    mListeners->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IConnectionListener> l = IConnectionListener::Probe(obj);
        l->OnLocalVideoCapabilityChanged(mLocalVideoCapable);
    }

    return NOERROR;
}

/**
 * Sets whether video capability is present remotely.
 *
 * @param capable {@code True} if video capable.
 */
ECode Connection::SetRemoteVideoCapable(
    /* [in] */ Boolean capable)
{
    mRemoteVideoCapable = capable;
    AutoPtr<IIterator> ator;
    mListeners->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IConnectionListener> l = IConnectionListener::Probe(obj);
        l->OnRemoteVideoCapabilityChanged(mRemoteVideoCapable);
    }
    return NOERROR;
}

/**
 * Set the audio quality for the connection.
 *
 * @param audioQuality The audio quality.
 */
ECode Connection::SetAudioQuality(
    /* [in] */ Int32 audioQuality)
{
    mAudioQuality = audioQuality;

    AutoPtr<IIterator> ator;
    mListeners->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IConnectionListener> l = IConnectionListener::Probe(obj);
        l->OnAudioQualityChanged(mAudioQuality);
    }

    return NOERROR;
}

/**
 * Sets the call substate for the current connection and reports the changes to all listeners.
 * Valid call substates are defined in {@link android.telecom.Connection}.
 *
 * @return The call substate.
 */
ECode Connection::SetCallSubstate(
    /* [in] */ Int32 callSubstate)
{
    mCallSubstate = callSubstate;

    AutoPtr<IIterator> ator;
    mListeners->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IConnectionListener> l = IConnectionListener::Probe(obj);
        l->OnCallSubstateChanged(mCallSubstate);
    }

    return NOERROR;
}

/**
 * Sets the {@link android.telecom.Connection.VideoProvider} for the connection.
 *
 * @param videoProvider The video call provider.
 */
ECode Connection::SetVideoProvider(
    /* [in] */ IConnectionVideoProvider* videoProvider)
{
    mVideoProvider = videoProvider;
    AutoPtr<IIterator> ator;
    mListeners->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IConnectionListener> l = IConnectionListener::Probe(obj);
        l->OnVideoProviderChanged(mVideoProvider);
    }

    return NOERROR;
}

ECode Connection::SetConverted(
    /* [in] */ const String& oriNumber)
{
    mNumberConverted = TRUE;
    mConvertedNumber = mAddress;
    mAddress = oriNumber;
    mDialString = oriNumber;
    return NOERROR;
}

/**
 * Notifies listeners of a change to conference participant(s).
 *
 * @param conferenceParticipants The participant(s).
 */
ECode Connection::UpdateConferenceParticipants(
    /* [in] */ IList* conferenceParticipants)
{
    AutoPtr<IIterator> ator;
    mListeners->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IConnectionListener> l = IConnectionListener::Probe(obj);
        l->OnConferenceParticipantsChanged(conferenceParticipants);
    }

    return NOERROR;
}

/**
 * Notifies this Connection of a request to disconnect a participant of the conference managed
 * by the connection.
 *
 * @param endpoint the {@link Uri} of the participant to disconnect.
 */
ECode Connection::OnDisconnectConferenceParticipant(
    /* [in] */ IUri* endpoint)
{
    return NOERROR;
}

/**
 * Build a human representation of a connection instance, suitable for debugging.
 * Don't log personal stuff unless in debug mode.
 * @return a string representing the internal state of this connection.
 */
ECode Connection::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder str;

    if (Logger::IsLoggable(TAG, Logger::___DEBUG)) {
        String v;
        str.Append(String("addr: ") + (GetAddress(&v), v));
        Int32 iv = 0;
        GetNumberPresentation(&iv);
        str.Append(String(" pres.: ") + StringUtils::ToString(iv));
        str.Append(String(" dial: ") + (GetOrigDialString(&v), v));
        str.Append(String(" postdial: ") + (GetRemainingPostDialString(&v), v));
        str.Append(String(" cnap name: ") + (GetCnapName(&v), v));
        GetCnapNamePresentation(&iv);
        str.Append(String("(") + StringUtils::ToString(iv) + String(")"));
    }
    Boolean bv = FALSE;
    IsIncoming(&bv);
    str.Append(String(" incoming: ") + StringUtils::ToString(bv));

    ICallState cs;
    GetState(&cs);
    str.Append(String(" state: ") + StringUtils::ToString(cs));
    IConnectionPostDialState ps;
    GetPostDialState(&ps);
    str.Append(String(" post dial state: ") + StringUtils::ToString(ps));
    return str.ToString(result);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
