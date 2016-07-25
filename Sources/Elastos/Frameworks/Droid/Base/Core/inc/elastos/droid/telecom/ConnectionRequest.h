#ifndef __ELASTOS_DROID_TELECOM_CONNECTIONREQUEST_H__
#define __ELASTOS_DROID_TELECOM_CONNECTIONREQUEST_H__

#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telecom.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecom {

/**
 * Simple data container encapsulating a request to some entity to
 * create a new {@link Connection}.
 * @hide
 */
class ConnectionRequest
    : public Object
    , public IConnectionRequest
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * @param accountHandle The accountHandle which should be used to place the call.
     * @param handle The handle (e.g., phone number) to which the {@link Connection} is to connect.
     * @param extras Application-specific extra data.
     */
    CARAPI constructor(
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [in] */ IUri* handle,
        /* [in] */ IBundle* extras);

    /**
     * @param accountHandle The accountHandle which should be used to place the call.
     * @param handle The handle (e.g., phone number) to which the {@link Connection} is to connect.
     * @param extras Application-specific extra data.
     * @param videoState Determines the video state for the connection.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [in] */ IUri* handle,
        /* [in] */ IBundle* extras,
        /* [in] */ Int32 videoState);

    /**
     * The account which should be used to place the call.
     */
    CARAPI GetAccountHandle(
        /* [out] */ IPhoneAccountHandle** result);

    /**
     * The handle (e.g., phone number) to which the {@link Connection} is to connect.
     */
    CARAPI GetAddress(
        /* [out] */ IUri** result);

    /**
     * Application-specific extra data. Used for passing back information from an incoming
     * call {@code Intent}, and for any proprietary extensions arranged between a client
     * and servant {@code ConnectionService} which agree on a vocabulary for such data.
     */
    CARAPI GetExtras(
        /* [out] */ IBundle** result);

    /**
     * Describes the video states supported by the client requesting the connection.
     * Valid values: {@link VideoProfile.VideoState#AUDIO_ONLY},
     * {@link VideoProfile.VideoState#BIDIRECTIONAL},
     * {@link VideoProfile.VideoState#TX_ENABLED},
     * {@link VideoProfile.VideoState#RX_ENABLED}.
     *
     * @return The video state for the connection.
     * @hide
     */
    CARAPI GetVideoState(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* destination);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

private:
    // TODO: Token to limit recursive invocations
    AutoPtr<IPhoneAccountHandle> mAccountHandle;
    AutoPtr<IUri> mAddress;
    AutoPtr<IBundle> mExtras;
    Int32 mVideoState;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CONNECTIONREQUEST_H__