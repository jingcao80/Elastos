#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CVIDEOCALLBACK_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CVIDEOCALLBACK_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Telecomm_Telecom_CVideoCallback.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Telecomm::Internal::IIVideoCallback;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CVideoCallback)
    , public Object
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
        /* [in] */ IVideoProfile* requestedProfile,
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
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CVIDEOCALLBACK_H__