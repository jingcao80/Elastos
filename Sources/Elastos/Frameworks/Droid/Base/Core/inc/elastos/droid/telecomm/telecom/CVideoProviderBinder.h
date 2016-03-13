#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CVIDEOPROVIDERBINDER_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CVIDEOPROVIDERBINDER_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Telecomm_Telecom_CVideoProviderBinder.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Telecomm::Internal::IIVideoProvider;
using Elastos::Droid::View::ISurface;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CVideoProviderBinder)
    , public Object
    , public IIVideoProvider
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IHandler* handler);

    CARAPI SetVideoCallback(
        /* [in] */ IBinder* videoCallbackBinder);

    CARAPI SetCamera(
        /* [in] */ const String& cameraId);

    CARAPI SetPreviewSurface(
        /* [in] */ ISurface* surface);

    CARAPI SetDisplaySurface(
        /* [in] */ ISurface* surface);

    CARAPI SetDeviceOrientation(
        /* [in] */ Int32 rotation);

    CARAPI SetZoom(
        /* [in] */ Float value);

    CARAPI SendSessionModifyRequest(
        /* [in] */ IVideoProfile* requestProfile);

    CARAPI SendSessionModifyResponse(
        /* [in] */ IVideoProfile* responseProfile);

    CARAPI RequestCameraCapabilities();

    CARAPI RequestCallDataUsage();

    CARAPI SetPauseImage(
        /* [in] */ const String& uri);

public:
    AutoPtr<IHandler> mMessageHandler;
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CVIDEOPROVIDERBINDER_H__