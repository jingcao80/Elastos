//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/hardware/camera2/legacy/CParameterUtils.h"
#include "elastos/droid/hardware/camera2/legacy/ParameterUtils.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(CParameterUtils, Singleton, IParameterUtils)

CAR_SINGLETON_IMPL(CParameterUtils)

ECode CParameterUtils::GetNORMALIZED_RECTANGLE_DEFAULT(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);

    return ParameterUtils::GetNORMALIZED_RECTANGLE_DEFAULT(rect);
}

ECode CParameterUtils::GetCAMERA_AREA_DEFAULT(
    /* [out] */ ICameraArea** area)
{
    VALIDATE_NOT_NULL(area);

    return ParameterUtils::GetCAMERA_AREA_DEFAULT(area);
}

ECode CParameterUtils::GetRECTANGLE_EMPTY(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);

    return ParameterUtils::GetRECTANGLE_EMPTY(rect);
}

ECode CParameterUtils::ConvertSize(
    /* [in] */ ICameraSize* size,
    /* [out] */ ISize** outsize)
{
    VALIDATE_NOT_NULL(outsize);

    return ParameterUtils::ConvertSize(size, outsize);
}

ECode CParameterUtils::ConvertSizeList(
    /* [in] */ IList* sizeList,
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist);

    return ParameterUtils::ConvertSizeList(sizeList, outlist);
}

ECode CParameterUtils::ConvertSizeListToArray(
    /* [in] */ IList* sizeList,
    /* [out, callee] */ ArrayOf<ISize*>** outarr)
{
    VALIDATE_NOT_NULL(outarr);

    return ParameterUtils::ConvertSizeListToArray(sizeList, outarr);
}

ECode CParameterUtils::ContainsSize(
    /* [in] */ IList* sizeList,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    return ParameterUtils::ContainsSize(sizeList, width, height, value);
}

ECode CParameterUtils::GetLargestSupportedJpegSizeByArea(
    /* [in] */ IParameters* params,
    /* [out] */ ISize** outsize)
{
    VALIDATE_NOT_NULL(outsize);

    return ParameterUtils::GetLargestSupportedJpegSizeByArea(params, outsize);
}

ECode CParameterUtils::StringFromArea(
    /* [in] */ ICameraArea* area,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    return ParameterUtils::StringFromArea(area, str);
}

ECode CParameterUtils::StringFromAreaList(
    /* [in] */ IList* areaList,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    return ParameterUtils::StringFromAreaList(areaList, str);
}

ECode CParameterUtils::GetClosestAvailableZoomCrop(
    /* [in] */ IParameters* params,
    /* [in] */ IRect* activeArray,
    /* [in] */ ISize* streamSize,
    /* [in] */ IRect* cropRegion,
    /* [in] */ IRect* reportedCropRegion,
    /* [in] */ IRect* previewCropRegion,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return ParameterUtils::GetClosestAvailableZoomCrop(params, activeArray, streamSize,
            cropRegion, reportedCropRegion, previewCropRegion, value);
}

ECode CParameterUtils::GetAvailableZoomCropRectangles(
    /* [in] */ IParameters* params,
    /* [in] */ IRect* activeArray,
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist);

    return ParameterUtils::GetAvailableZoomCropRectangles(params, activeArray, outlist);
}

ECode CParameterUtils::GetAvailablePreviewZoomCropRectangles(
    /* [in] */ IParameters* params,
    /* [in] */ IRect* activeArray,
    /* [in] */ ISize* previewSize,
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist);

    return ParameterUtils::GetAvailablePreviewZoomCropRectangles(params, activeArray,
            previewSize, outlist);
}

ECode CParameterUtils::GetMaxZoomRatio(
    /* [in] */ IParameters* params,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    return ParameterUtils::GetMaxZoomRatio(params, value);
}

ECode CParameterUtils::ConvertScalerCropRegion(
    /* [in] */ IRect* activeArraySize,
    /* [in] */ IRect* cropRegion,
    /* [in] */ ISize* previewSize,
    /* [in] */ IParameters* params,
    /* [out] */ IParameterUtilsZoomData** outzd)
{
    VALIDATE_NOT_NULL(outzd);

    return ParameterUtils::ConvertScalerCropRegion(activeArraySize, cropRegion, previewSize,
            params, outzd);
}

ECode CParameterUtils::ConvertMeteringRectangleToLegacy(
    /* [in] */ IRect* activeArray,
    /* [in] */ IMeteringRectangle* meteringRect,
    /* [in] */ IParameterUtilsZoomData* zoomData,
    /* [out] */ IParameterUtilsMeteringData** outmd)
{
    VALIDATE_NOT_NULL(outmd);

    return ParameterUtils::ConvertMeteringRectangleToLegacy(activeArray, meteringRect,
            zoomData, outmd);
}

ECode CParameterUtils::ConvertCameraAreaToActiveArrayRectangle(
    /* [in] */ IRect* activeArray,
    /* [in] */ IParameterUtilsZoomData* zoomData,
    /* [in] */ ICameraArea* area,
    /* [out] */ IParameterUtilsWeightedRectangle** outwr)
{
    VALIDATE_NOT_NULL(outwr);

    return ParameterUtils::ConvertCameraAreaToActiveArrayRectangle(activeArray, zoomData,
            area, outwr);
}

ECode CParameterUtils::ConvertFaceFromLegacy(
    /* [in] */ ICameraFace* face,
    /* [in] */ IRect* activeArray,
    /* [in] */ IParameterUtilsZoomData* zoomData,
    /* [out] */ IFace** outface)
{
    VALIDATE_NOT_NULL(outface);

    return ParameterUtils::ConvertFaceFromLegacy(face, activeArray, zoomData, outface);
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
