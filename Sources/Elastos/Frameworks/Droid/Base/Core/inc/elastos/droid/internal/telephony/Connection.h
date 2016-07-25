
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CONNECTION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CONNECTION_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Telecomm::Telecom::IConnectionVideoProvider;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * {@hide}
 */
class ECO_PUBLIC Connection
    : public Object
    , public IConnection
{
public:
    /**
     * Base listener implementation.
     */
    class ListenerBase
        : public Object
        , public IConnectionListener
    {
    public:
        CAR_INTERFACE_DECL()

        virtual ~ListenerBase() = 0;

        // @Override
        CARAPI OnVideoStateChanged(
            /* [in] */ Int32 videoState);

        // @Override
        CARAPI OnLocalVideoCapabilityChanged(
            /* [in] */ Boolean capable);

        // @Override
        CARAPI OnRemoteVideoCapabilityChanged(
            /* [in] */ Boolean capable);

        // @Override
        CARAPI OnVideoProviderChanged(
            /* [in] */ IConnectionVideoProvider* videoProvider);

        // @Override
        CARAPI OnAudioQualityChanged(
            /* [in] */ Int32 audioQuality);

        // @Override
        CARAPI OnCallSubstateChanged(
            /* [in] */ Int32 callSubstate);

        // @Override
        CARAPI OnConferenceParticipantsChanged(
            /* [in] */ IList* participants);
    };

public:
    CAR_INTERFACE_DECL()

    Connection();

    virtual ~Connection();

    /* Instance Methods */
    /**
     * Gets address (e.g. phone number) associated with connection.
     * TODO: distinguish reasons for unavailability
     *
     * @return address or NULL if unavailable
     */
    CARAPI GetAddress(
        /* [out] */ String* result);

    /**
     * Gets CNAP name associated with connection.
     * @return cnap name or NULL if unavailable
     */
    CARAPI GetCnapName(
        /* [out] */ String* result);

    /**
     * Get original dial string.
     * @return original dial string or NULL if unavailable
     */
    CARAPI GetOrigDialString(
        /* [out] */ String* result);

    /**
     * Gets CNAP presentation associated with connection.
     * @return cnap name or NULL if unavailable
     */

    CARAPI GetCnapNamePresentation(
        /* [out] */ Int32* result);

    /**
     * Connection create time in currentTimeMillis() format
     * Basically, set when object is created.
     * Effectively, when an incoming call starts ringing or an
     * outgoing call starts dialing
     */
    CARAPI GetCreateTime(
        /* [out] */ Int64* result);

    /**
     * Connection connect time in currentTimeMillis() format.
     * For outgoing calls: Begins at (DIALING|ALERTING) -> ACTIVE transition.
     * For incoming calls: Begins at (INCOMING|WAITING) -> ACTIVE transition.
     * Returns 0 before then.
     */
    CARAPI GetConnectTime(
        /* [out] */ Int64* result);

    /**
     * Connection connect time in elapsedRealtime() format.
     * For outgoing calls: Begins at (DIALING|ALERTING) -> ACTIVE transition.
     * For incoming calls: Begins at (INCOMING|WAITING) -> ACTIVE transition.
     * Returns 0 before then.
     */
    CARAPI GetConnectTimeReal(
        /* [out] */ Int64* result);

    /**
     * Returns the number of milliseconds the call has been connected,
     * or 0 if the call has never connected.
     * If the call is still connected, then returns the elapsed
     * time since connect.
     */
    CARAPI GetDurationMillis(
        /* [out] */ Int64* result);

    /**
     * The time when this Connection last transitioned into HOLDING
     * in elapsedRealtime() format.
     * Returns 0, if it has never made a transition into HOLDING.
     */
    CARAPI GetHoldingStartTime(
        /* [out] */ Int64* result);

    /**
     * Returns TRUE of this connection originated elsewhere
     * ("MT" or mobile terminated; another party called this terminal)
     * or FALSE if this call originated here (MO or mobile originated).
     */
    CARAPI IsIncoming(
        /* [out] */ Boolean* result);

    /**
     * If this Connection is connected, then it is associated with
     * a Call.
     *
     * Returns getCall().getState() or Call.State.IDLE if not
     * connected
     */
    CARAPI GetState(
        /* [out] */ ICallState* result);

    /**
     * If this connection went through handover return the state of the
     * call that contained this connection before handover.
     */
    CARAPI GetStateBeforeHandover(
        /* [out] */ ICallState* result);

    /**
     * Get the extras for the connection's call.
     *
     * Returns getCall().getExtras()
     */
    CARAPI GetExtras(
        /* [out] */ IBundle** extras);

    /**
     * isAlive()
     *
     * @return TRUE if the connection isn't disconnected
     * (could be active, holding, ringing, dialing, etc)
     */
    CARAPI IsAlive(
        /* [out] */ Boolean* result);

    /**
     * Returns TRUE if Connection is connected and is INCOMING or WAITING
     */
    CARAPI IsRinging(
        /* [out] */ Boolean* result);

    /**
     *
     * @return the userdata set in setUserData()
     */
    CARAPI GetUserData(
        /* [out] */ IInterface** result);

    /**
     *
     * @param userdata user can store an any userdata in the Connection object.
     */
    CARAPI SetUserData(
        /* [in] */ IInterface* userdata);

    CARAPI ClearUserData();

    CARAPI AddPostDialListener(
        /* [in] */ IConnectionPostDialListener* listener);

    /**
     * Returns the original Connection instance associated with
     * this Connection
     */
    CARAPI GetOrigConnection(
        /* [out] */ IConnection** result);

    CARAPI MigrateFrom(
        /* [in] */ IConnection* c);

    /**
     * Assign a listener to be notified of state changes.
     *
     * @param listener A listener.
     */
    CARAPI AddListener(
        /* [in] */ IConnectionListener* listener);

    /**
     * Removes a listener.
     *
     * @param listener A listener.
     */
    CARAPI RemoveListener(
        /* [in] */ IConnectionListener* listener);

    /**
     * Returns the current video state of the connection.
     *
     * @return The video state of the connection.
     */
    CARAPI GetVideoState(
        /* [out] */ Int32* result);

    /**
     * Returns the local video capability state for the connection.
     *
     * @return {@code True} if the connection has local video capabilities.
     */
    CARAPI IsLocalVideoCapable(
        /* [out] */ Boolean* result);

    /**
     * Returns the remote video capability state for the connection.
     *
     * @return {@code True} if the connection has remote video capabilities.
     */
    CARAPI IsRemoteVideoCapable(
        /* [out] */ Boolean* result);

    /**
     * Returns the {@link android.telecom.Connection.VideoProvider} for the connection.
     *
     * @return The {@link android.telecom.Connection.VideoProvider}.
     */
    CARAPI GetVideoProvider(
        /* [out] */ IConnectionVideoProvider** result);

    /**
     * Returns the audio-quality for the connection.
     *
     * @return The audio quality for the connection.
     */
    CARAPI GetAudioQuality(
        /* [out] */ Int32* result);

    /**
     * Returns the current call substate of the connection.
     *
     * @return The call substate of the connection.
     */
    CARAPI GetCallSubstate(
        /* [out] */ Int32* result);

    /**
     * Sets the videoState for the current connection and reports the changes to all listeners.
     * Valid video states are defined in {@link android.telecom.VideoProfile}.
     *
     * @return The video state.
     */
    CARAPI SetVideoState(
        /* [in] */ Int32 videoState);

    /**
     * Sets whether video capability is present locally.
     *
     * @param capable {@code True} if video capable.
     */
    CARAPI SetLocalVideoCapable(
        /* [in] */ Boolean capable);

    /**
     * Sets whether video capability is present remotely.
     *
     * @param capable {@code True} if video capable.
     */
    CARAPI SetRemoteVideoCapable(
        /* [in] */ Boolean capable);

    /**
     * Set the audio quality for the connection.
     *
     * @param audioQuality The audio quality.
     */
    CARAPI SetAudioQuality(
        /* [in] */ Int32 audioQuality);

    /**
     * Sets the call substate for the current connection and reports the changes to all listeners.
     * Valid call substates are defined in {@link android.telecom.Connection}.
     *
     * @return The call substate.
     */
    CARAPI SetCallSubstate(
        /* [in] */ Int32 callSubstate);

    /**
     * Sets the {@link android.telecom.Connection.VideoProvider} for the connection.
     *
     * @param videoProvider The video call provider.
     */
    CARAPI SetVideoProvider(
        /* [in] */ IConnectionVideoProvider* videoProvider);

    CARAPI SetConverted(
        /* [in] */ const String& oriNumber);

    /**
     * Notifies listeners of a change to conference participant(s).
     *
     * @param conferenceParticipants The participant(s).
     */
    CARAPI UpdateConferenceParticipants(
        /* [in] */ IList* conferenceParticipants);

    /**
     * Notifies this Connection of a request to disconnect a participant of the conference managed
     * by the connection.
     *
     * @param endpoint the {@link Uri} of the participant to disconnect.
     */
    CARAPI OnDisconnectConferenceParticipant(
        /* [in] */ IUri* endpoint);

    /**
     * Build a human representation of a connection instance, suitable for debugging.
     * Don't log personal stuff unless in debug mode.
     * @return a string representing the internal state of this connection.
     */
    CARAPI ToString(
        /* [out] */ String* result);

protected:
    CARAPI_(void) ClearPostDialListeners();

    CARAPI_(void) NotifyPostDialListeners();

public:
    static const Int32 AUDIO_QUALITY_STANDARD;
    static const Int32 AUDIO_QUALITY_HIGH_DEFINITION;
    AutoPtr<ISet> mListeners;
    ICallState mPreHandoverState;

protected:
    //Caller Name Display
    String mCnapName;
    Int32 mCnapNamePresentation;
    String mAddress;     // MAY BE NULL!!!
    String mDialString;          // outgoing calls only
    Int32 mNumberPresentation;
    Boolean mIsIncoming;
    /*
     * These time/timespan values are based on System.currentTimeMillis(),
     * i.e., "wall clock" time.
     */
    Int64 mCreateTime;
    Int64 mConnectTime;
    /*
     * These time/timespan values are based on SystemClock.elapsedRealTime(),
     * i.e., time since boot.  They are appropriate for comparison and
     * calculating deltas.
     */
    Int64 mConnectTimeReal;
    Int64 mDuration;
    Int64 mHoldingStartTime;  // The time when the Connection last transitioned
                            // into HOLDING
    AutoPtr<IConnection> mOrigConnection;
    AutoPtr<IList> mPostDialListeners; //List<IConnectionPostDialListener>

    Boolean mNumberConverted;
    String mConvertedNumber;

    AutoPtr<IInterface> mUserData;

private:
    static String TAG;
    Int32 mVideoState;
    Boolean mLocalVideoCapable;
    Boolean mRemoteVideoCapable;
    Int32 mAudioQuality;
    Int32 mCallSubstate;
    AutoPtr<IConnectionVideoProvider> mVideoProvider;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CONNECTION_H__
