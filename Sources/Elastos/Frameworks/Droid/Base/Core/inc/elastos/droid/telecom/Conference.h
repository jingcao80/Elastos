#ifndef __ELASTOS_DROID_TELECOM_CONFERENCE_H__
#define __ELASTOS_DROID_TELECOM_CONFERENCE_H__

#include "Elastos.Droid.Telecom.h"
#include "elastos/droid/telecom/Connection.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Telecom {

/**
 * Represents a conference call which can contain any number of {@link Connection} objects.
 * @hide
 */
class ECO_PUBLIC Conference
    : public Object
    , public IConference
{
public:
    class ConnectionListener
        : public Connection::Listener
    {
    public:
        ConnectionListener(
            /* [in] */ Conference* host);

        CARAPI OnDestroyed(
            /* [in] */ IConnection* c);

    public:
        Conference* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    Conference();

    /**
     * Constructs a new Conference with a mandatory {@link PhoneAccountHandle}
     *
     * @param phoneAccount The {@code PhoneAccountHandle} associated with the conference.
     */
    CARAPI constructor(
        /* [in] */ IPhoneAccountHandle* phoneAccount);

    /**
     * Returns the {@link PhoneAccountHandle} the conference call is being placed through.
     *
     * @return A {@code PhoneAccountHandle} object representing the PhoneAccount of the conference.
     */
    CARAPI GetPhoneAccountHandle(
        /* [out] */ IPhoneAccountHandle** result);

    /**
     * Returns the list of connections currently associated with the conference call.
     *
     * @return A list of {@code Connection} objects which represent the children of the conference.
     */
    CARAPI GetConnections(
        /* [out] */ IList** res);

    /**
     * Gets the state of the conference call. See {@link Connection} for valid values.
     *
     * @return A constant representing the state the conference call is currently in.
     */
    CARAPI GetState(
        /* [out] */ Int32* res);

    /**
     * Returns the capabilities of a conference. See {@link PhoneCapabilities} for valid values.
     *
     * @return A bitmask of the {@code PhoneCapabilities} of the conference call.
     */
    CARAPI GetCapabilities(
        /* [out] */ Int32* res);

    /**
     * @return The audio state of the conference, describing how its audio is currently
     *         being routed by the system. This is {@code null} if this Conference
     *         does not directly know about its audio state.
     */
    CARAPI GetAudioState(
        /* [out] */ IAudioState** res);

    /**
     * Invoked when the Conference and all it's {@link Connection}s should be disconnected.
     */
    CARAPI OnDisconnect();

    /**
     * Invoked when the specified {@link Connection} should be separated from the conference call.
     *
     * @param connection The connection to separate.
     */
    CARAPI OnSeparate(
        /* [in] */ IConnection* connection);

    /**
     * Invoked when the specified {@link Connection} should merged with the conference call.
     *
     * @param connection The {@code Connection} to merge.
     */
    CARAPI OnMerge(
        /* [in] */ IConnection* connection);

    /**
     * Invoked when the conference should be put on hold.
     */
    CARAPI OnHold();

    /**
     * Invoked when the conference should be moved from hold to active.
     */
    CARAPI OnUnhold();

    /**
     * Invoked when the child calls should be merged. Only invoked if the conference contains the
     * capability {@link PhoneCapabilities#MERGE_CONFERENCE}.
     */
    CARAPI OnMerge();

    /**
     * Invoked when the child calls should be swapped. Only invoked if the conference contains the
     * capability {@link PhoneCapabilities#SWAP_CONFERENCE}.
     */
    CARAPI OnSwap();

    /**
     * Notifies this conference of a request to play a DTMF tone.
     *
     * @param c A DTMF character.
     */
    CARAPI OnPlayDtmfTone(
        /* [in] */ Char32 c);

    /**
     * Notifies this conference of a request to stop any currently playing DTMF tones.
     */
    CARAPI OnStopDtmfTone();

    /**
     * Notifies this conference that the {@link #getAudioState()} property has a new value.
     *
     * @param state The new call audio state.
     */
    CARAPI OnAudioStateChanged(
        /* [in] */ IAudioState* state);

    /**
     * Sets state to be on hold.
     */
    CARAPI SetOnHold();

    /**
     * Sets state to be active.
     */
    CARAPI SetActive();

    /**
     * Sets state to disconnected.
     *
     * @param disconnectCause The reason for the disconnection, as described by
     *     {@link android.telecom.DisconnectCause}.
     */
    CARAPI SetDisconnected(
        /* [in] */ IDisconnectCause* disconnectCause);

    /**
     * Sets the capabilities of a conference. See {@link PhoneCapabilities} for valid values.
     *
     * @param capabilities A bitmask of the {@code PhoneCapabilities} of the conference call.
     */
    CARAPI SetCapabilities(
        /* [in] */ Int32 capabilities);

    /**
     * Adds the specified connection as a child of this conference.
     *
     * @param connection The connection to add.
     * @return True if the connection was successfully added.
     */
    CARAPI AddConnection(
        /* [in] */ IConnection* connection,
        /* [out] */ Boolean* res);

    /**
     * Removes the specified connection as a child of this conference.
     *
     * @param connection The connection to remove.
     */
    CARAPI RemoveConnection(
        /* [in] */ IConnection* connection);

    /**
     * Sets the connections with which this connection can be conferenced.
     *
     * @param conferenceableConnections The set of connections this connection can conference with.
     */
    CARAPI SetConferenceableConnections(
        /* [in] */ IList* conferenceableConnections);

    /**
     * Returns the connections with which this connection can be conferenced.
     */
    CARAPI GetConferenceableConnections(
        /* [out] */ IList** res);

    /**
     * Tears down the conference object and any of its current connections.
     */
    CARAPI Destroy();

    /**
     * Add a listener to be notified of a state change.
     *
     * @param listener The new listener.
     * @return This conference.
     * @hide
     */
    CARAPI AddListener(
        /* [in] */ IConferenceListener* listener);

    /**
     * Removes the specified listener.
     *
     * @param listener The listener to remove.
     * @return This conference.
     * @hide
     */
    CARAPI RemoveListener(
        /* [in] */ IConferenceListener* listener);

    /**
     * Inform this Conference that the state of its audio output has been changed externally.
     *
     * @param state The new audio state.
     * @hide
     */
    CARAPI SetAudioState(
        /* [in] */ IAudioState* state);

    CARAPI_(void) FireOnConferenceableConnectionsChanged();

    CARAPI_(void) SetState(
        /* [in] */ Int32 newState);

    CARAPI_(void) ClearConferenceableList();

private:
    AutoPtr<ISet> mListeners;
    AutoPtr<IList> mChildConnections;
    AutoPtr<IList> mUnmodifiableChildConnections;
    AutoPtr<IList> mConferenceableConnections;
    AutoPtr<IList> mUnmodifiableConferenceableConnections;

    AutoPtr<IPhoneAccountHandle> mPhoneAccount;
    AutoPtr<IAudioState> mAudioState;
    Int32 mState;
    AutoPtr<IDisconnectCause> mDisconnectCause;
    Int32 mCapabilities;
    String mDisconnectMessage;

    AutoPtr<IConnectionListener> mConnectionDeathListener;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CONFERENCE_H__