#ifndef __ELASTOS_DROID_TELECOM_CVIDEOCALLLISTENERBINDER_H__
#define __ELASTOS_DROID_TELECOM_CVIDEOCALLLISTENERBINDER_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Telecom_CVideoCallListenerBinder.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Telecom::IIVideoCallback;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecom {

/**
 * IVideoCallback stub implementation.
 */
CarClass(CVideoCallListenerBinder)
    , public Object
    , public IVideoCallListenerBinder
    , public IIVideoCallback
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IHandler* handler);

    CARAPI ReceiveSessionModifyRequest(
        /* [in] */ IVideoProfile* videoProfile);

    CARAPI ReceiveSessionModifyResponse(
        /* [in] */ Int32 status,
        /* [in] */ IVideoProfile* requestProfile,
        /* [in] */ IVideoProfile* responseProfile);

    CARAPI HandleCallSessionEvent(
        /* [in] */ Int32 event);

    CARAPI ChangePeerDimensions(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI ChangeCallDataUsage(
        /* [in] */ Int32 dataUsage);

    CARAPI ChangeCameraCapabilities(
        /* [in] */ ICameraCapabilities* cameraCapabilities);

public:
    AutoPtr<IHandler> mHandler;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CVIDEOCALLLISTENERBINDER_H__