#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_PARCELABLECALL_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_PARCELABLECALL_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telecomm.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Telecomm::Internal::IIVideoProvider;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * Information about a call that is used between InCallService and Telecom.
 * @hide
 */
class ParcelableCall
    : public Object
    , public IParcelableCall
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    ParcelableCall();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ String id,
        /* [in] */ Int32 state,
        /* [in] */ IDisconnectCause* disconnectCause,
        /* [in] */ IList* cannedSmsResponses,
        /* [in] */ Int32 capabilities,
        /* [in] */ Int32 properties,
        /* [in] */ Int64 connectTimeMillis,
        /* [in] */ IUri* handle,
        /* [in] */ Int32 handlePresentation,
        /* [in] */ String callerDisplayName,
        /* [in] */ Int32 callerDisplayNamePresentation,
        /* [in] */ IGatewayInfo* gatewayInfo,
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [in] */ IIVideoProvider* videoCallProvider,
        /* [in] */ String parentCallId,
        /* [in] */ IList* childCallIds,
        /* [in] */ IStatusHints* statusHints,
        /* [in] */ Int32 videoState,
        /* [in] */ IList* conferenceableCallIds,
        /* [in] */ IBundle* extras);

    /** The unique ID of the call. */
    CARAPI GetId(
        /* [out] */ String* result);

    /** The current state of the call. */
    CARAPI GetState(
        /* [out] */ Int32* result);

    /**
     * Reason for disconnection, as described by {@link android.telecomm.DisconnectCause}. Valid
     * when call state is {@link CallState#DISCONNECTED}.
     */
    CARAPI GetDisconnectCause(
        /* [out] */ IDisconnectCause** result);

    /**
     * The set of possible text message responses when this call is incoming.
     */
    CARAPI GetCannedSmsResponses(
        /* [out] */ IList** result);

    // Bit mask of actions a call supports, values are defined in {@link CallCapabilities}.
    CARAPI GetCapabilities(
        /* [out] */ Int32* result);

    /** Bitmask of properties of the call. */
    CARAPI GetProperties(
        /* [out] */ Int32* result);

    /** The time that the call switched to the active state. */
    CARAPI GetConnectTimeMillis(
        /* [out] */ Int64* result);

    /** The endpoint to which the call is connected. */
    CARAPI GetHandle(
        /* [out] */ IUri** result);

    /**
     * The presentation requirements for the handle. See {@link TelecomManager} for valid values.
     */
    CARAPI GetHandlePresentation(
        /* [out] */ Int32* result);

    /** The endpoint to which the call is connected. */
    CARAPI GetCallerDisplayName(
        /* [out] */ String* result);

    /**
     * The presentation requirements for the caller display name.
     * See {@link TelecomManager} for valid values.
     */
    CARAPI GetCallerDisplayNamePresentation(
        /* [out] */ Int32* result);

    /** Gateway information for the call. */
    CARAPI GetGatewayInfo(
        /* [out] */ IGatewayInfo** result);

    /** PhoneAccountHandle information for the call. */
    CARAPI GetAccountHandle(
        /* [out] */ IPhoneAccountHandle** result);

    /**
     * Returns an object for remotely communicating through the video call provider's binder.
     * @return The video call.
     */
    CARAPI GetVideoCall(
        /* [out] */ IInCallServiceVideoCall** result);

    /**
     * The conference call to which this call is conferenced. Null if not conferenced.
     */
    CARAPI GetParentCallId(
        /* [out] */ String* result);

    /**
     * The child call-IDs if this call is a conference call. Returns an empty list if this is not
     * a conference call or if the conference call contains no children.
     */
    CARAPI GetChildCallIds(
        /* [out] */ IList** result);

    CARAPI GetConferenceableCallIds(
        /* [out] */ IList** result);

    /**
     * The status label and icon.
     *
     * @return Status hints.
     */
    CARAPI GetStatusHints(
        /* [out] */ IStatusHints** result);

    /**
     * The video state.
     * @return The video state of the call.
     */
    CARAPI GetVideoState(
        /* [out] */ Int32* result);

    /**
     * Any extras to pass with the call
     *
     * @return a bundle of extras
     */
    CARAPI GetExtras(
        /* [out] */ IBundle** result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /** Writes ParcelableCall object into a Parcel. */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* destination);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    String mId;
    Int32 mState;
    AutoPtr<IDisconnectCause> mDisconnectCause;
    AutoPtr<IList> mCannedSmsResponses;
    Int32 mCapabilities;
    Int32 mProperties;
    Int64 mConnectTimeMillis;
    AutoPtr<IUri> mHandle;
    Int32 mHandlePresentation;
    String mCallerDisplayName;
    Int32 mCallerDisplayNamePresentation;
    AutoPtr<IGatewayInfo> mGatewayInfo;
    AutoPtr<IPhoneAccountHandle> mAccountHandle;
    AutoPtr<IIVideoProvider> mVideoCallProvider;
    AutoPtr<IInCallServiceVideoCall> mVideoCall;
    String mParentCallId;
    AutoPtr<IList> mChildCallIds;
    AutoPtr<IStatusHints> mStatusHints;
    Int32 mVideoState;
    AutoPtr<IList> mConferenceableCallIds;
    AutoPtr<IBundle> mExtras;
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_PARCELABLECALL_H__