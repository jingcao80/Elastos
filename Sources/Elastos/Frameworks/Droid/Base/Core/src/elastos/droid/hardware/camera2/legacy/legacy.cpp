
#include "elastos/droid/hardware/camera2/legacy/CBurstHolder.h"
#include "elastos/droid/hardware/camera2/legacy/CCameraDeviceState.h"
#include "elastos/droid/hardware/camera2/legacy/CCameraDeviceUserShim.h"
#include "elastos/droid/hardware/camera2/legacy/CCaptureCollector.h"
#include "elastos/droid/hardware/camera2/legacy/CGLThreadManager.h"
#include "elastos/droid/hardware/camera2/legacy/CLegacyCameraDevice.h"
#include "elastos/droid/hardware/camera2/legacy/CLegacyFaceDetectMapper.h"
#include "elastos/droid/hardware/camera2/legacy/CLegacyFocusStateMapper.h"
#include "elastos/droid/hardware/camera2/legacy/CLegacyRequest.h"
#include "elastos/droid/hardware/camera2/legacy/CLegacyResultMapper.h"
#include "elastos/droid/hardware/camera2/legacy/CPerfMeasurement.h"
#include "elastos/droid/hardware/camera2/legacy/CRequestHandlerThread.h"
#include "elastos/droid/hardware/camera2/legacy/CRequestHolderBuilder.h"
#include "elastos/droid/hardware/camera2/legacy/CRequestQueue.h"
#include "elastos/droid/hardware/camera2/legacy/CRequestThreadManager.h"
#include "elastos/droid/hardware/camera2/legacy/CRequestThreadManagerFpsCounter.h"
#include "elastos/droid/hardware/camera2/legacy/CLegacySizeAreaComparator.h"
#include "elastos/droid/hardware/camera2/legacy/CSurfaceTextureRenderer.h"
#include "elastos/droid/hardware/camera2/legacy/CZoomData.h"
#include "elastos/droid/hardware/camera2/legacy/CMeteringData.h"
#include "elastos/droid/hardware/camera2/legacy/CWeightedRectangle.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_OBJECT_IMPL(CBurstHolder)
CAR_OBJECT_IMPL(CCameraDeviceState)
CAR_OBJECT_IMPL(CCameraDeviceUserShim)
CAR_OBJECT_IMPL(CCaptureCollector)
CAR_OBJECT_IMPL(CGLThreadManager)
CAR_OBJECT_IMPL(CLegacyCameraDevice)
CAR_OBJECT_IMPL(CLegacyFaceDetectMapper)
CAR_OBJECT_IMPL(CLegacyFocusStateMapper)
CAR_OBJECT_IMPL(CLegacyRequest)
CAR_OBJECT_IMPL(CLegacyResultMapper)
CAR_OBJECT_IMPL(CPerfMeasurement)
CAR_OBJECT_IMPL(CRequestHandlerThread)
CAR_OBJECT_IMPL(CRequestHolderBuilder)
CAR_OBJECT_IMPL(CRequestQueue)
CAR_OBJECT_IMPL(CRequestThreadManager)
CAR_OBJECT_IMPL(CRequestThreadManagerFpsCounter)
CAR_OBJECT_IMPL(CLegacySizeAreaComparator)
CAR_OBJECT_IMPL(CSurfaceTextureRenderer)
CAR_OBJECT_IMPL(CZoomData)
CAR_OBJECT_IMPL(CMeteringData)
CAR_OBJECT_IMPL(CWeightedRectangle)

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos