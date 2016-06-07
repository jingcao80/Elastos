#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_PARCELABLECONNECTION_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_PARCELABLECONNECTION_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Telecomm.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Telecomm::Internal::IIVideoProvider;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * Information about a connection that is used between Telecom and the ConnectionService.
 * This is used to send initial Connection information to Telecom when the connection is
 * first created.
 * @hide
 */
class ParcelableConnection
    : public Object
    , public IParcelableConnection
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    ParcelableConnection();

    CARAPI constructor();

    /** @hide */
    CARAPI constructor(
        /* [in] */ IPhoneAccountHandle* phoneAccount,
        /* [in] */ Int32 state,
        /* [in] */ Int32 capabilities,
        /* [in] */ IUri* address,
        /* [in] */ Int32 addressPresentation,
        /* [in] */ const String& callerDisplayName,
        /* [in] */ Int32 callerDisplayNamePresentation,
        /* [in] */ IIVideoProvider* videoProvider,
        /* [in] */ Int32 videoState,
        /* [in] */ Boolean ringbackRequested,
        /* [in] */ Boolean isVoipAudioMode,
        /* [in] */ IStatusHints* statusHints,
        /* [in] */ IDisconnectCause* disconnectCause,
        /* [in] */ IList* conferenceableConnectionIds);

    CARAPI GetPhoneAccount(
        /* [out] */ IPhoneAccountHandle** result);

    CARAPI GetState(
        /* [out] */ Int32* result);

    // Bit mask of actions a call supports, values are defined in {@link CallCapabilities}.
    CARAPI GetCapabilities(
        /* [out] */ Int32* result);

    CARAPI GetProperties(
        /* [out] */ Int32* res);

    CARAPI GetHandle(
        /* [out] */ IUri** result);

    CARAPI GetHandlePresentation(
        /* [out] */ Int32* result);

    CARAPI GetCallerDisplayName(
        /* [out] */ String* result);

    CARAPI GetCallerDisplayNamePresentation(
        /* [out] */ Int32* result);

    CARAPI GetVideoProvider(
        /* [out] */ IIVideoProvider** result);

    CARAPI GetVideoState(
        /* [out] */ Int32* result);

    CARAPI IsRingbackRequested(
        /* [out] */ Boolean* result);

    CARAPI GetIsVoipAudioMode(
        /* [out] */ Boolean* result);

    CARAPI GetStatusHints(
        /* [out] */ IStatusHints** result);

    CARAPI GetDisconnectCause(
        /* [out] */ IDisconnectCause** result);

    CARAPI GetConferenceableConnectionIds(
        /* [out] */ IList** result);

    CARAPI GetCallSubstate(
        /* [out] */ Int32* res);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /** Writes ParcelableConnection object into a Parcel. */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* destination);

private:
    AutoPtr<IPhoneAccountHandle> mPhoneAccount;
    Int32 mState;
    Int32 mCapabilities;
    Int32 mProperties;
    AutoPtr<IUri> mAddress;
    Int32 mAddressPresentation;
    String mCallerDisplayName;
    Int32 mCallerDisplayNamePresentation;
    AutoPtr<IIVideoProvider> mVideoProvider;
    Int32 mVideoState;
    Boolean mRingbackRequested;
    Boolean mIsVoipAudioMode;
    AutoPtr<IStatusHints> mStatusHints;
    AutoPtr<IDisconnectCause> mDisconnectCause;
    AutoPtr<IList> mConferenceableConnectionIds;
    Int32 mCallSubstate;
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_PARCELABLECONNECTION_H__