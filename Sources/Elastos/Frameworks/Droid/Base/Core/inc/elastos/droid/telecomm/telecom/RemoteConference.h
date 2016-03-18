#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_REMOTECONFERENCE_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_REMOTECONFERENCE_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Telecomm.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Telecomm::Internal::IIConnectionService;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * Represents a conference call which can contain any number of {@link Connection} objects.
 * @hide
 */
class RemoteConference
    : public Object
    , public IRemoteConference
{
public:
    CAR_INTERFACE_DECL()

    RemoteConference();

    /** {@hide} */
    CARAPI constructor(
        /* [in] */ const String& id,
        /* [in] */ IIConnectionService* connectionService);

    /** {@hide} */
    CARAPI GetId(
        /* [out] */ String* result);

    /** {@hide} */
    CARAPI SetDestroyed();

    /** {@hide} */
    CARAPI SetState(
        /* [in] */ Int32 newState);

    /** {@hide} */
    CARAPI AddConnection(
        /* [in] */ IRemoteConnection* connection);

    /** {@hide} */
    CARAPI RemoveConnection(
        /* [in] */ IRemoteConnection* connection);

    /** {@hide} */
    CARAPI SetCallCapabilities(
        /* [in] */ Int32 capabilities);

    /** @hide */
    CARAPI SetConferenceableConnections(
        /* [in] */ IList* conferenceableConnections);

    /** {@hide} */
    CARAPI SetDisconnected(
        /* [in] */ IDisconnectCause* disconnectCause);

    CARAPI GetConnections(
        /* [out] */ IList** result);

    CARAPI GetState(
        /* [out] */ Int32* result);

    CARAPI GetCallCapabilities(
        /* [out] */ Int32* result);

    CARAPI Disconnect();

    CARAPI Separate(
        /* [in] */ IRemoteConnection* connection);

    CARAPI Merge();

    CARAPI Swap();

    CARAPI Hold();

    CARAPI Unhold();

    CARAPI GetDisconnectCause(
        /* [out] */ IDisconnectCause** result);

    CARAPI PlayDtmfTone(
        /* [in] */ Char32 digit);

    CARAPI StopDtmfTone();

    CARAPI SetAudioState(
        /* [in] */ IAudioState* state);

    CARAPI GetConferenceableConnections(
        /* [out] */ IList** result);

    CARAPI RegisterCallback(
        /* [in] */ IRemoteConferenceCallback* clb);

    CARAPI UnregisterCallback(
        /* [in] */ IRemoteConferenceCallback* clb);

private:
    String mId;
    AutoPtr<IIConnectionService> mConnectionService;

    AutoPtr<ISet> mCallbacks;
    AutoPtr<IList> mChildConnections;
    AutoPtr<IList> mUnmodifiableChildConnections;
    AutoPtr<IList> mConferenceableConnections;
    AutoPtr<IList> mUnmodifiableConferenceableConnections;

    Int32 mState;
    AutoPtr<IDisconnectCause> mDisconnectCause;
    Int32 mCallCapabilities;
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_REMOTECONFERENCE_H__