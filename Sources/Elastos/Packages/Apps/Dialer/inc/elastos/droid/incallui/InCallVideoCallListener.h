
#ifndef __ELASTOS_DROID_INCALLUI_INCALLVIDEOCALLLISTENER_H__
#define __ELASTOS_DROID_INCALLUI_INCALLVIDEOCALLLISTENER_H__

#include "Elastos.Droid.Telecom.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/incallui/Call.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Telecom::ICameraCapabilities;
using Elastos::Droid::Telecom::IVideoCallListener;
using Elastos::Droid::Telecom::IVideoProfile;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class Call;

class InCallVideoCallListener
    : public Object
    , public IVideoCallListener
{
public:
    CAR_INTERFACE_DECL();

    /**
     * Creates an instance of the call video client, specifying the call it is related to.
     *
     * @param call The call.
     */
    InCallVideoCallListener(
        /* [in] */ Call* call);

    /**
     * Handles an incoming session modification request.
     *
     * @param videoProfile The requested video call profile.
     */
    // @Override
    CARAPI OnSessionModifyRequestReceived(
        /* [in] */ IVideoProfile* videoProfile);

    /**
     * Handles a session modification response.
     *
     * @param status Status of the session modify request.  Valid values are
     *               {@link Connection.VideoProvider#SESSION_MODIFY_REQUEST_SUCCESS},
     *               {@link Connection.VideoProvider#SESSION_MODIFY_REQUEST_FAIL},
     *               {@link Connection.VideoProvider#SESSION_MODIFY_REQUEST_INVALID}
     * @param requestedProfile
     * @param responseProfile The actual profile changes made by the peer device.
     */
    // @Override
    CARAPI OnSessionModifyResponseReceived(
        /* [in] */ Int32 status,
        /* [in] */ IVideoProfile* requestedProfile,
        /* [in] */ IVideoProfile* responseProfile);

    /**
     * Handles a call session event.
     *
     * @param event The event.
     */
    // @Override
    CARAPI OnCallSessionEvent(
        /* [in] */ Int32 event);

    /**
     * Handles a change to the peer video dimensions.
     *
     * @param width  The updated peer video width.
     * @param height The updated peer video height.
     */
    // @Override
    CARAPI OnPeerDimensionsChanged(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Handles a change to the call data usage.  No implementation as the in-call UI does not
     * display data usage.
     *
     * @param dataUsage The updated data usage.
     */
    // @Override
    CARAPI OnCallDataUsageChanged(
        /* [in] */ Int32 dataUsage);

    /**
     * Handles changes to the camera capabilities.  No implementation as the in-call UI does not
     * make use of camera capabilities.
     *
     * @param cameraCapabilities The changed camera capabilities.
     */
    // @Override
    CARAPI OnCameraCapabilitiesChanged(
        /* [in] */ ICameraCapabilities* cameraCapabilities);
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_INCALLVIDEOCALLLISTENER_H__
