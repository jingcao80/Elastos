
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYRESULTMAPPER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYRESULTMAPPER_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::IParameters;
using Elastos::Droid::Hardware::Camera2::ICameraCharacteristics;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Legacy::IParameterUtilsZoomData;
using Elastos::Droid::Hardware::Camera2::Legacy::ILegacyRequest;
using Elastos::Droid::Hardware::Camera2::Params::IMeteringRectangle;
using Elastos::Droid::Graphics::IRect;
using Elastos::Core::Object;
using Elastos::Core::IArrayOf;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class LegacyResultMapper
    : public Object
    , public ILegacyResultMapper
{
public:
    CAR_INTERFACE_DECL()

    LegacyResultMapper();

    virtual ~LegacyResultMapper() {}

    /**
     * Generate capture result metadata from the legacy camera request.
     *
     * <p>This method caches and reuses the result from the previous call to this method if
     * the {@code parameters} of the subsequent {@link LegacyRequest} passed to this method
     * have not changed.</p>
     *
     * @param legacyRequest a non-{@code null} legacy request containing the latest parameters
     * @param timestamp the timestamp to use for this result in nanoseconds.
     *
     * @return {@link CameraMetadataNative} object containing result metadata.
     */
    CARAPI CachedConvertResultMetadata(
        /* [in] */ ILegacyRequest* legacyRequest,
        /* [in] */ Int64 timestamp,
        /* [out] */ ICameraMetadataNative** outcmn);

private:
    /**
     * Generate capture result metadata from the legacy camera request.
     *
     * @param legacyRequest a non-{@code null} legacy request containing the latest parameters
     * @return a {@link CameraMetadataNative} object containing result metadata.
     */
    static CARAPI_(AutoPtr<ICameraMetadataNative>) ConvertResultMetadata(
        /* [in] */ ILegacyRequest* legacyRequest);

    static CARAPI_(void) MapAe(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ ICameraCharacteristics* characteristics,
        /* [in] */ ICaptureRequest* request,
        /* [in] */ IRect* activeArray,
        /* [in] */ IParameterUtilsZoomData* zoomData,
        /* [in] */ IParameters* p);

    static CARAPI_(void) MapAf(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IRect* activeArray,
        /* [in] */ IParameterUtilsZoomData* zoomData,
        /* [in] */ IParameters* p);

    static CARAPI_(void) MapAwb(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IParameters* p);

    static CARAPI_(AutoPtr<IArrayOf> /* IMeteringRectangle */) GetMeteringRectangles(
        /* [in] */ IRect* activeArray,
        /* [in] */ IParameterUtilsZoomData* zoomData,
        /* [in] */ ArrayOf<ICameraArea*>* meteringAreaList,
        /* [in] */ const String& regionName);

     /** Map results for control.aeMode, flash.mode, flash.state */
    static CARAPI_(void) MapAeAndFlashMode(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ ICameraCharacteristics* characteristics,
        /* [in] */ IParameters* p);

    static CARAPI_(Int32) ConvertLegacyAfMode(
        /* [in] */ const String& mode);

    static CARAPI_(Int32) ConvertLegacyAwbMode(
        /* [in] */ const String& mode);

    /** Map results for scaler.* */
    static CARAPI_(void) MapScaler(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IParameterUtilsZoomData* zoomData,
        /* [in] */ IParameters* p);

private:
    static const String TAG;
    static const Boolean VERBOSE;

    AutoPtr<ILegacyRequest> mCachedRequest;
    AutoPtr<ICameraMetadataNative> mCachedResult;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYRESULTMAPPER_H__
