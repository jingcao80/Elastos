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

#include "elastos/droid/hardware/camera2/legacy/ParameterUtils.h"
#include "elastos/droid/hardware/camera2/legacy/CZoomData.h"
#include "elastos/droid/hardware/camera2/legacy/CMeteringData.h"
#include "elastos/droid/hardware/camera2/legacy/CWeightedRectangle.h"
#include "elastos/droid/hardware/camera2/utils/UtilsSizeAreaComparator.h"
#include "elastos/droid/hardware/camera2/utils/ListUtils.h"
#include "elastos/droid/hardware/camera2/utils/ParamsUtils.h"
#include "elastos/droid/hardware/camera2/params/CMeteringRectangle.h"
#include "elastos/droid/hardware/camera2/params/CFace.h"
#include "elastos/droid/hardware/HardwareCamera.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/graphics/CPoint.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/utility/CSize.h"
#include "elastos/droid/utility/CSizeF.h"
#include <elastos/core/Math.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Camera2::Legacy::CZoomData;
using Elastos::Droid::Hardware::Camera2::Legacy::CMeteringData;
using Elastos::Droid::Hardware::Camera2::Legacy::CWeightedRectangle;
using Elastos::Droid::Hardware::Camera2::Params::CFace;
using Elastos::Droid::Hardware::Camera2::Params::CMeteringRectangle;
using Elastos::Droid::Hardware::Camera2::Utils::UtilsSizeAreaComparator;
using Elastos::Droid::Hardware::Camera2::Utils::ListUtils;
using Elastos::Droid::Hardware::Camera2::Utils::ParamsUtils;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Utility::CSize;
using Elastos::Droid::Utility::CSizeF;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Core::Math;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

static AutoPtr<IRect> InitNORMALIZED_RECTANGLE_DEFAULT()
{
    AutoPtr<IRect> rect;
    CRect::New(
        IParameterUtils::NORMALIZED_RECTANGLE_MIN,
        IParameterUtils::NORMALIZED_RECTANGLE_MIN,
        IParameterUtils::NORMALIZED_RECTANGLE_MAX,
        IParameterUtils::NORMALIZED_RECTANGLE_MAX,
        (IRect**)&rect);
    return rect;
}

const AutoPtr<IRect> ParameterUtils::NORMALIZED_RECTANGLE_DEFAULT = InitNORMALIZED_RECTANGLE_DEFAULT();

static AutoPtr<ICameraArea> InitCAMERA_AREA_DEFAULT()
{
    AutoPtr<IRect> rect = InitNORMALIZED_RECTANGLE_DEFAULT();
    AutoPtr<ICameraArea> area = new HardwareCamera::Area(rect, /*weight*/1);
    return area;
}

const AutoPtr<ICameraArea> ParameterUtils::CAMERA_AREA_DEFAULT = InitCAMERA_AREA_DEFAULT();

static AutoPtr<IRect> InitRECTANGLE_EMPTY()
{
    AutoPtr<IRect> rect;
    CRect::New(/*left*/0, /*top*/0, /*right*/0, /*bottom*/0, (IRect**)&rect);
    return rect;
}

const AutoPtr<IRect> ParameterUtils::RECTANGLE_EMPTY = InitRECTANGLE_EMPTY();

const String ParameterUtils::TAG("ParameterUtils");
const Boolean ParameterUtils::VERBOSE = FALSE;//Log.isLoggable(TAG, Log.VERBOSE);

/** getZoomRatios stores zoom ratios in 1/100 increments, e.x. a zoom of 3.2 is 320 */
const Int32 ParameterUtils::ZOOM_RATIO_MULTIPLIER = 100;

ECode ParameterUtils::GetNORMALIZED_RECTANGLE_DEFAULT(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);

    *rect = NORMALIZED_RECTANGLE_DEFAULT;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

ECode ParameterUtils::GetCAMERA_AREA_DEFAULT(
    /* [out] */ ICameraArea** area)
{
    VALIDATE_NOT_NULL(area);

    *area = CAMERA_AREA_DEFAULT;
    REFCOUNT_ADD(*area);
    return NOERROR;
}

ECode ParameterUtils::GetRECTANGLE_EMPTY(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);

    *rect = RECTANGLE_EMPTY;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

CAR_INTERFACE_IMPL(ParameterUtils::ZoomData, Object, IParameterUtilsZoomData)

ParameterUtils::ZoomData::ZoomData()
    :mZoomIndex(0)
{
}

ECode ParameterUtils::ZoomData::constructor()
{
    return NOERROR;
}

ECode ParameterUtils::ZoomData::constructor(
    /* [in] */ Int32 zoomIndex,
    /* [in] */ IRect* previewCrop,
    /* [in] */ IRect* reportedCrop)
{
    mZoomIndex = zoomIndex;
    mPreviewCrop = previewCrop;
    mReportedCrop = reportedCrop;
    return NOERROR;
}

ECode ParameterUtils::ZoomData::GetZoomIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    *index = mZoomIndex;
    return NOERROR;
}

ECode ParameterUtils::ZoomData::GetPreviewCrop(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);

    *rect = mPreviewCrop;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

ECode ParameterUtils::ZoomData::GetReportedCrop(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);

    *rect = mReportedCrop;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

CAR_INTERFACE_IMPL(ParameterUtils::MeteringData, Object, IParameterUtilsMeteringData)

ParameterUtils::MeteringData::MeteringData()
{
}

ECode ParameterUtils::MeteringData::constructor()
{
    return NOERROR;
}

ECode ParameterUtils::MeteringData::constructor(
    /* [in] */ ICameraArea* meteringArea,
    /* [in] */ IRect* previewMetering,
    /* [in] */ IRect* reportedMetering)
{
    mMeteringArea = meteringArea;
    mPreviewMetering = previewMetering;
    mReportedMetering = reportedMetering;
    return NOERROR;
}

ECode ParameterUtils::MeteringData::GetMeteringArea(
    /* [out] */ ICameraArea** area)
{
    VALIDATE_NOT_NULL(area);

    *area = mMeteringArea;
    REFCOUNT_ADD(*area);
    return NOERROR;
}

ECode ParameterUtils::MeteringData::GetPreviewMetering(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);

    *rect = mPreviewMetering;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

ECode ParameterUtils::MeteringData::GetReportedMetering(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);

    *rect = mReportedMetering;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

CAR_INTERFACE_IMPL(ParameterUtils::WeightedRectangle, Object, IParameterUtilsWeightedRectangle)

ParameterUtils::WeightedRectangle::WeightedRectangle()
    :mWeight(0)
{
}

ECode ParameterUtils::WeightedRectangle::constructor()
{
    return NOERROR;
}

ECode ParameterUtils::WeightedRectangle::constructor(
    /* [in] */ IRect* rect,
    /* [in] */ Int32 weight)
{
    FAIL_RETURN(Preconditions::CheckNotNull(rect, String("rect must not be null")))
    mRect = rect;
    mWeight = weight;
    return NOERROR;
}

ECode ParameterUtils::WeightedRectangle::ToMetering(
    /* [out] */ IMeteringRectangle** outmr)
{
    VALIDATE_NOT_NULL(outmr);

    Int32 weight = Clip(mWeight,
            IMeteringRectangle::METERING_WEIGHT_MIN,
            IMeteringRectangle::METERING_WEIGHT_MAX,
            mRect,
            String("weight"));

    Int32 left, top, width, height;
    mRect->GetLeft(&left);
    Int32 x = ClipLower(left, /*lo*/0, mRect, String("left"));
    mRect->GetTop(&top);
    Int32 y = ClipLower(top, /*lo*/0, mRect, String("top"));
    mRect->GetWidth(&width);
    Int32 w = ClipLower(width, /*lo*/0, mRect, String("width"));
    mRect->GetHeight(&height);
    Int32 h = ClipLower(height, /*lo*/0, mRect, String("height"));
    return CMeteringRectangle::New(x, y, w, h, weight, outmr);
}

ECode ParameterUtils::WeightedRectangle::ToFace(
    /* [in] */ Int32 id,
    /* [in] */ IPoint* leftEyePosition,
    /* [in] */ IPoint* rightEyePosition,
    /* [in] */ IPoint* mouthPosition,
    /* [out] */ IFace** outface)
{
    VALIDATE_NOT_NULL(outface);

    Int32 idSafe = ClipLower(id, /*lo*/0, mRect, String("id"));
    Int32 score = Clip(mWeight,
            IFace::SCORE_MIN,
            IFace::SCORE_MAX,
            mRect,
            String("score"));

    return CFace::New(mRect, score, idSafe, leftEyePosition, rightEyePosition, mouthPosition, outface);
}

ECode ParameterUtils::WeightedRectangle::ToFace(
    /* [out] */ IFace** outface)
{
    VALIDATE_NOT_NULL(outface);

    Int32 score = Clip(mWeight,
            IFace::SCORE_MIN,
            IFace::SCORE_MAX,
            mRect,
            String("score"));

    return CFace::New(mRect, score, outface);
}

ECode ParameterUtils::WeightedRectangle::GetRect(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);

    *rect = mRect;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

ECode ParameterUtils::WeightedRectangle::GetWeight(
    /* [out] */ Int32* weight)
{
    VALIDATE_NOT_NULL(weight);

    *weight = mWeight;
    return NOERROR;
}

Int32 ParameterUtils::WeightedRectangle::ClipLower(
    /* [in] */ Int32 value,
    /* [in] */ Int32 lo,
    /* [in] */ IRect* rect,
    /* [in] */ const String& name)
{
    return Clip(value, lo, /*hi*/Elastos::Core::Math::INT32_MAX_VALUE, rect, name);
}

Int32 ParameterUtils::WeightedRectangle::Clip(
    /* [in] */ Int32 value,
    /* [in] */ Int32 lo,
    /* [in] */ Int32 hi,
    /* [in] */ IRect* rect,
    /* [in] */ const String& name)
{
    if (value < lo) {
        String str;
        IObject::Probe(rect)->ToString(&str);
        Logger::W(TAG, "toMetering - Rectangle %s %s too small, clip to %d", str.string(), name.string(), lo);
        value = lo;
    }
    else if (value > hi) {
        String str;
        IObject::Probe(rect)->ToString(&str);
        Logger::W(TAG, "toMetering - Rectangle %s %s too small, clip to %d",str.string(), name.string(), hi);
        value = hi;
    }

    return value;
}

ParameterUtils::ParameterUtils()
{
}

ECode ParameterUtils::ConvertSize(
    /* [in] */ ICameraSize* size,
    /* [out] */ ISize** outsize)
{
    VALIDATE_NOT_NULL(outsize)
    *outsize = NULL;
    FAIL_RETURN(Preconditions::CheckNotNull(size, String("size must not be null")))

    Int32 width;
    size->GetWidth(&width);
    Int32 height;
    size->GetHeight(&height);
    return CSize::New(width, height, outsize);
}

ECode ParameterUtils::ConvertSizeList(
    /* [in] */ IList* sizeList,
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist)
    *outlist = NULL;
    FAIL_RETURN(Preconditions::CheckNotNull(sizeList, String("sizeList must not be null")))

    Int32 size;
    sizeList->GetSize(&size);
    AutoPtr<IList> sizes;
    CArrayList::New(size, (IList**)&sizes);

    Int32 width, height;
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        sizeList->Get(i, (IInterface**)&obj);
        AutoPtr<ICameraSize> s = ICameraSize::Probe(obj);

        s->GetWidth(&width);
        s->GetHeight(&height);
        AutoPtr<ISize> tmp;
        CSize::New(width, height, (ISize**)&tmp);
        sizes->Add(tmp);
    }
    *outlist = sizes;
    REFCOUNT_ADD(*outlist);
    return NOERROR;
}

ECode ParameterUtils::ConvertSizeArrayToList(
    /* [in] */ ArrayOf<ICameraSize*>* sizeList,
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist)
    *outlist = NULL;
    FAIL_RETURN(Preconditions::CheckNotNull(sizeList, String("sizeList must not be null")))

    AutoPtr<IList> sizes;
    CArrayList::New(sizeList->GetLength(), (IList**)&sizes);

    Int32 width, height;
    for (Int32 i = 0; i < sizeList->GetLength(); i++) {
        (*sizeList)[i]->GetWidth(&width);
        (*sizeList)[i]->GetHeight(&height);
        AutoPtr<ISize> tmp;
        CSize::New(width, height, (ISize**)&tmp);
        sizes->Add(tmp);
    }
    *outlist = sizes;
    REFCOUNT_ADD(*outlist);
    return NOERROR;
}

ECode ParameterUtils::ConvertSizeListToArray(
    /* [in] */ IList* sizeList,
    /* [out, callee] */ ArrayOf<ISize*>** outarr)
{
    VALIDATE_NOT_NULL(outarr)
    *outarr = NULL;
    FAIL_RETURN(Preconditions::CheckNotNull(sizeList, String("sizeList must not be null")))

    Int32 size;
    sizeList->GetSize(&size);
    AutoPtr<ArrayOf<ISize*> > array = ArrayOf<ISize*>::Alloc(size);

    Int32 width, height;
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        sizeList->Get(i, (IInterface**)&obj);
        AutoPtr<ICameraSize> s = ICameraSize::Probe(obj);

        s->GetWidth(&width);
        s->GetHeight(&height);
        AutoPtr<ISize> tmp;
        CSize::New(width, height, (ISize**)&tmp);
        array->Set(i, tmp);
    }
    *outarr = array;
    REFCOUNT_ADD(*outarr);
    return NOERROR;
}

ECode ParameterUtils::ConvertSizeArrayToArray(
    /* [in] */ ArrayOf<ICameraSize*>* sizeList,
    /* [out, callee] */ ArrayOf<ISize*>** outarr)
{
    VALIDATE_NOT_NULL(outarr)
    *outarr = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(sizeList, String("sizeList must not be null")))

    Int32 size = sizeList->GetLength();
    AutoPtr<ArrayOf<ISize*> > array = ArrayOf<ISize*>::Alloc(size);

    Int32 width, height;
    for (Int32 i = 0; i < size; i++) {
        (*sizeList)[i]->GetWidth(&width);
        (*sizeList)[i]->GetHeight(&height);
        AutoPtr<ISize> tmp;
        CSize::New(width, height, (ISize**)&tmp);
        array->Set(i, tmp);
    }
    *outarr = array;
    REFCOUNT_ADD(*outarr);
    return NOERROR;
}

ECode ParameterUtils::ContainsSize(
    /* [in] */ IList* sizeList,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;
    FAIL_RETURN(Preconditions::CheckNotNull(sizeList, String("sizeList must not be null")))

    Int32 size;
    sizeList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        sizeList->Get(i, (IInterface**)&obj);
        AutoPtr<ICameraSize> s = ICameraSize::Probe(obj);

        Int32 _width, _height;
        s->GetWidth(&_width);
        s->GetHeight(&_height);
        if (_height == height && _width == width) {
            *value = TRUE;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode ParameterUtils::ContainsSize(
    /* [in] */ ArrayOf<ICameraSize*>* sizeList,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;
    FAIL_RETURN(Preconditions::CheckNotNull(sizeList, String("sizeList must not be null")))

    Int32 _width, _height;
    Int32 size = sizeList->GetLength();
    for (Int32 i = 0; i < size; i++) {
        (*sizeList)[i]->GetWidth(&_width);
        (*sizeList)[i]->GetHeight(&_height);
        if (_height == height && _width == width) {
            *value = TRUE;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode ParameterUtils::GetLargestSupportedJpegSizeByArea(
    /* [in] */ IParameters* params,
    /* [out] */ ISize** outsize)
{
    VALIDATE_NOT_NULL(outsize)
    *outsize = NULL;
    FAIL_RETURN(Preconditions::CheckNotNull(params, String("params must not be null")))

    AutoPtr<ArrayOf<ICameraSize*> > sizes;
    params->GetSupportedPictureSizes((ArrayOf<ICameraSize*>**)&sizes);
    AutoPtr<IList> tmpList;
    Arrays::AsList(sizes, (IList**)&tmpList);
    AutoPtr<IList> supportedJpegSizes;
    ConvertSizeList(tmpList, (IList**)&supportedJpegSizes);
    return UtilsSizeAreaComparator::FindLargestByArea(supportedJpegSizes, outsize);
}

ECode ParameterUtils::StringFromArea(
    /* [in] */ ICameraArea* area,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    if (area == NULL) {
        *str= NULL;
        return NOERROR;
    }
    else {
        StringBuilder sb;
        AutoPtr<IRect> r;
        area->GetRect((IRect**)&r);

        sb.SetLength(0);
        sb += "([";
        Int32 left;
        r->GetLeft(&left);
        sb += left;
        sb += ',';
        Int32 top;
        r->GetTop(&top);
        sb += top;
        sb += "][";
        Int32 right;
        r->GetRight(&right);
        sb += right;
        sb += ',';
        Int32 bottom;
        r->GetBottom(&bottom);
        sb += bottom;
        sb += ']';

        sb += ',';
        Int32 weight;
        area->GetWeight(&weight);
        sb += weight;
        sb += ')';

        return sb.ToString(str);
    }
}

ECode ParameterUtils::StringFromAreaList(
    /* [in] */ IList* areaList,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder sb;

    if (areaList == NULL) {
        *str = NULL;
        return NOERROR;
    }

    Int32 size;
    areaList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        areaList->Get(i, (IInterface**)&obj);
        AutoPtr<ICameraArea> area = ICameraArea::Probe(obj);

        if (area == NULL) {
            sb += "null";
        }
        else {
            String str;
            StringFromArea(area, &str);
            sb += str;
        }

        if (i != size - 1) {
            sb += ", ";
        }
    }

    return sb.ToString(str);
}

ECode ParameterUtils::GetClosestAvailableZoomCrop(
    /* [in] */ IParameters* params,
    /* [in] */ IRect* activeArray,
    /* [in] */ ISize* streamSize,
    /* [in] */ IRect* cropRegion,
    /* [in] */ IRect* reportedCropRegion,
    /* [in] */ IRect* previewCropRegion,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    FAIL_RETURN(Preconditions::CheckNotNull(params, String("params must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(activeArray, String("activeArray must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(streamSize, String("streamSize must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(reportedCropRegion, String("reportedCropRegion must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(previewCropRegion, String("previewCropRegion must not be null")))

    AutoPtr<IRect> actualCrop;
    CRect::New(cropRegion, (IRect**)&actualCrop);

    /*
     * Shrink requested crop region to fit inside of the active array size
     */
    Boolean result;
    actualCrop->Intersect(activeArray, &result);
    if (!result) {
        Logger::W(TAG, "getClosestAvailableZoomCrop - Crop region out of range;"
            "setting to active array size");
        actualCrop->Set(activeArray);
    }

    AutoPtr<IRect> previewCrop;
    FAIL_RETURN(GetPreviewCropRectangleUnzoomed(activeArray, streamSize, (IRect**)&previewCrop))

    // Make the user-requested crop region the same aspect ratio as the preview stream size
    AutoPtr<IRect> cropRegionAsPreview;
    ShrinkToSameAspectRatioCentered(previewCrop, actualCrop, (IRect**)&cropRegionAsPreview);

    if (VERBOSE) {
        String str;
        IObject::Probe(actualCrop)->ToString(&str);
        Logger::V(TAG, "getClosestAvailableZoomCrop - actualCrop = %s", str.string());
        String str2;
        IObject::Probe(previewCrop)->ToString(&str2);
        Logger::V(TAG,
                "getClosestAvailableZoomCrop - previewCrop = %s", str2.string());
        String str3;
        IObject::Probe(previewCrop)->ToString(&str3);
        Logger::V(TAG,
                "getClosestAvailableZoomCrop - cropRegionAsPreview = %s", str3.string());
    }

    /*
     * Iterate all available zoom rectangles and find the closest zoom index
     */
    AutoPtr<IRect> bestReportedCropRegion;
    AutoPtr<IRect> bestPreviewCropRegion;
    Int32 bestZoomIndex = -1;

    AutoPtr<IList> availableReportedCropRegions;
    GetAvailableZoomCropRectangles(params, activeArray, (IList**)&availableReportedCropRegions);
    AutoPtr<IList> availablePreviewCropRegions;
    GetAvailablePreviewZoomCropRectangles(params, activeArray, streamSize, (IList**)&availablePreviewCropRegions);

    if (VERBOSE) {
        String str;
        ListUtils::ListToString(availableReportedCropRegions, &str);
        Logger::V(TAG,
                "getClosestAvailableZoomCrop - availableReportedCropRegions = %s", str.string());
        String str2;
        ListUtils::ListToString(availablePreviewCropRegions, &str2);
        Logger::V(TAG,
                "getClosestAvailableZoomCrop - availablePreviewCropRegions = %s", str2.string());
    }

    Int32 size;
    availableReportedCropRegions->GetSize(&size);
    Int32 size2;
    availablePreviewCropRegions->GetSize(&size2);
    if (size != size2) {
        //throw new AssertionError("available reported/preview crop region size mismatch");
        Logger::E(TAG, "available reported/preview crop region size mismatch");
        return E_ASSERTION_ERROR;
    }

    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        availablePreviewCropRegions->Get(i, (IInterface**)&obj);
        AutoPtr<IRect> currentPreviewCropRegion = IRect::Probe(obj);
        AutoPtr<IInterface> obj2;
        availableReportedCropRegions->Get(i, (IInterface**)&obj2);
        AutoPtr<IRect> currentReportedCropRegion = IRect::Probe(obj2);

        Boolean isBest;
        if (bestZoomIndex == -1) {
            isBest = TRUE;
        }
        else {
            Int32 width;
            currentPreviewCropRegion->GetWidth(&width);
            Int32 width2;
            cropRegionAsPreview->GetWidth(&width2);
            Int32 height;
            currentPreviewCropRegion->GetHeight(&height);
            Int32 height2;
            cropRegionAsPreview->GetHeight(&height2);
            if (width >= width2 && height >= height2) {
                isBest = TRUE;
            }
            else {
                isBest = FALSE;
            }
        }

        // Sizes are sorted largest-to-smallest, so once the available crop is too small,
        // we the rest are too small. Furthermore, this is the final best crop,
        // since its the largest crop that still fits the requested crop
        if (isBest) {
            bestPreviewCropRegion = currentPreviewCropRegion;
            bestReportedCropRegion = currentReportedCropRegion;
            bestZoomIndex = i;
        }
        else {
            break;
        }
    }

    if (bestZoomIndex == -1) {
        // Even in the worst case, we should always at least return 0 here
        //throw new AssertionError("Should've found at least one valid zoom index");
        Logger::E(TAG, "Should've found at least one valid zoom index");
        return E_ASSERTION_ERROR;
    }

    // Write the rectangles in-place
    reportedCropRegion->Set(bestReportedCropRegion);
    previewCropRegion->Set(bestPreviewCropRegion);

    *value = bestZoomIndex;
    return NOERROR;
}

ECode ParameterUtils::GetPreviewCropRectangleUnzoomed(
    /* [in] */ IRect* activeArray,
    /* [in] */ ISize* previewSize,
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);
    *rect = NULL;

    Int32 width;
    previewSize->GetWidth(&width);
    Int32 width2;
    activeArray->GetWidth(&width2);
    Int32 height;
    previewSize->GetHeight(&height);
    Int32 height2;
    activeArray->GetHeight(&height2);
    if (width > width2) {
        //throw new IllegalArgumentException("previewSize must not be wider than activeArray");
        Logger::E(TAG, "previewSize must not be wider than activeArray");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    }
    else if (height > height2) {
        //throw new IllegalArgumentException("previewSize must not be taller than activeArray");
        Logger::E(TAG, "previewSize must not be taller than activeArray");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Float aspectRatioArray = width2 * 1.0f / height2;
    Float aspectRatioPreview = width * 1.0f / height;

    Float cropH;
    Float cropW;
    if (aspectRatioPreview < aspectRatioArray) {
        // The new width must be smaller than the height, so scale the width by AR
        cropH = height2;
        cropW = cropH * aspectRatioPreview;
    }
    else {
        // The new height must be smaller (or equal) than the width, so scale the height by AR
        cropW = width2;
        cropH = cropW / aspectRatioPreview;
    }

    AutoPtr<IMatrix> translateMatrix;
    CMatrix::New((IMatrix**)&translateMatrix);
    AutoPtr<IRectF> cropRect;
    CRectF::New(/*left*/0, /*top*/0, cropW, cropH, (IRectF**)&cropRect);

    // Now center the crop rectangle so its center is in the center of the active array
    Float x;
    activeArray->GetExactCenterX(&x);
    Float y;
    activeArray->GetExactCenterY(&y);
    translateMatrix->SetTranslate(x, y);
    Float cx;
    cropRect->GetCenterX(&cx);
    Float cy;
    cropRect->GetCenterY(&cy);
    Boolean result;
    translateMatrix->PostTranslate(-cx, -cy, &result);

    translateMatrix->MapRect(/*inout*/cropRect, &result);

    // Round the rect corners towards the nearest integer values
    return ParamsUtils::CreateRect(cropRect, rect);
}

ECode ParameterUtils::ShrinkToSameAspectRatioCentered(
    /* [in] */ IRect* reference,
    /* [in] */ IRect* shrinkTarget,
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);
    *rect = NULL;

    Int32 width;
    reference->GetWidth(&width);
    Int32 width2;
    shrinkTarget->GetWidth(&width2);
    Int32 height;
    reference->GetHeight(&height);
    Int32 height2;
    shrinkTarget->GetHeight(&height2);

    Float aspectRatioReference = width * 1.0f / height;
    Float aspectRatioShrinkTarget = width2 * 1.0f / height2;

    Float cropH;
    Float cropW;
    if (aspectRatioShrinkTarget < aspectRatioReference) {
        // The new width must be smaller than the height, so scale the width by AR
        cropH = height;
        cropW = cropH * aspectRatioShrinkTarget;
    }
    else {
        // The new height must be smaller (or equal) than the width, so scale the height by AR
        cropW = width;
        cropH = cropW / aspectRatioShrinkTarget;
    }

    AutoPtr<IMatrix> translateMatrix;
    CMatrix::New((IMatrix**)&translateMatrix);
    AutoPtr<IRectF> shrunkRect;
    CRectF::New(shrinkTarget, (IRectF**)&shrunkRect);

    // Scale the rectangle down, but keep its center in the same place as before
    Float x;
    shrinkTarget->GetExactCenterX(&x);
    Float y;
    shrinkTarget->GetExactCenterY(&y);
    translateMatrix->SetScale(cropW / width, cropH / height, x, y);

    Boolean result;
    translateMatrix->MapRect(/*inout*/shrunkRect, &result);

    return ParamsUtils::CreateRect(shrunkRect, rect);
}

ECode ParameterUtils::GetAvailableZoomCropRectangles(
    /* [in] */ IParameters* params,
    /* [in] */ IRect* activeArray,
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist);
    *outlist = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(params, String("params must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(activeArray, String("activeArray must not be null")))

    AutoPtr<ISize> size;
    ParamsUtils::CreateSize(activeArray, (ISize**)&size);
    return GetAvailableCropRectangles(params, activeArray, size, outlist);
}

ECode ParameterUtils::GetAvailablePreviewZoomCropRectangles(
    /* [in] */ IParameters* params,
    /* [in] */ IRect* activeArray,
    /* [in] */ ISize* previewSize,
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist);
    *outlist = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(params, String("params must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(activeArray, String("activeArray must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(previewSize, String("previewSize must not be null")))

    return GetAvailableCropRectangles(params, activeArray, previewSize, outlist);
}

ECode ParameterUtils::GetAvailableCropRectangles(
    /* [in] */ IParameters* params,
    /* [in] */ IRect* activeArray,
    /* [in] */ ISize* streamSize,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(params, String("params must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(activeArray, String("activeArray must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(streamSize, String("streamSize must not be null")))

    // TODO: change all uses of Rect activeArray to Size activeArray,
    // since we want the crop to be active-array relative, not pixel-array relative

    AutoPtr<IRect> unzoomedStreamCrop;
    GetPreviewCropRectangleUnzoomed(activeArray, streamSize, (IRect**)&unzoomedStreamCrop);

    Boolean result;
    params->IsZoomSupported(&result);
    if (!result) {
        // Trivial case: No zoom -> only support the full size as the crop region
        AutoPtr<ArrayOf<IRect*> > array = ArrayOf<IRect*>::Alloc(1);
        array->Set(0, unzoomedStreamCrop);
        AutoPtr<IList> _list;
        Arrays::AsList(array, (IList**)&_list);
        *list = _list;
        REFCOUNT_ADD(*list);
        return NOERROR;
    }

    Int32 zoom;
    params->GetMaxZoom(&zoom);
    AutoPtr<IList> zoomCropRectangles;
    CArrayList::New(zoom + 1, (IList**)&zoomCropRectangles);

    AutoPtr<IMatrix> scaleMatrix;
    CMatrix::New((IMatrix**)&scaleMatrix);
    AutoPtr<IRectF> scaledRect;
    CRectF::New((IRectF**)&scaledRect);

    AutoPtr<ArrayOf<IInteger32*> > ratios;
    params->GetZoomRatios((ArrayOf<IInteger32*>**)&ratios);
    for (Int32 i = 0; i < ratios->GetLength(); i++) {
        AutoPtr<IInteger32> obj = (*ratios)[i];
        Int32 zoom;
        obj->GetValue(&zoom);

        Float shrinkRatio = ZOOM_RATIO_MULTIPLIER * 1.0f / zoom; // normalize to 1.0 and smaller

        // set scaledRect to unzoomedStreamCrop
        ParamsUtils::ConvertRectF(unzoomedStreamCrop, /*out*/scaledRect);

        Float x;
        activeArray->GetExactCenterX(&x);
        Float y;
        activeArray->GetExactCenterY(&y);
        scaleMatrix->SetScale(shrinkRatio, shrinkRatio, x, y);

        Boolean result;
        scaleMatrix->MapRect(scaledRect, &result);

        AutoPtr<IRect> intRect;
        ParamsUtils::CreateRect(scaledRect, (IRect**)&intRect);

        // Round the rect corners towards the nearest integer values
        zoomCropRectangles->Add(TO_IINTERFACE(intRect));
    }

    *list = zoomCropRectangles;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode ParameterUtils::GetMaxZoomRatio(
    /* [in] */ IParameters* params,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    Boolean result;
    params->IsZoomSupported(&result);
    if (!result) {
        *value = 1.0f; // no zoom
        return NOERROR;
    }

    AutoPtr<ArrayOf<IInteger32*> > zoomRatios;
    params->GetZoomRatios((ArrayOf<IInteger32*>**)&zoomRatios); // sorted smallest->largest
    AutoPtr<IInteger32> zoomObj = (*zoomRatios)[zoomRatios->GetLength() - 1];
    Int32 zoom;
    zoomObj->GetValue(&zoom); // largest zoom ratio
    Float zoomRatio = zoom * 1.0f / ZOOM_RATIO_MULTIPLIER; // normalize to 1.0 and smaller
    *value = zoomRatio;
    return NOERROR;
}

ECode ParameterUtils::GetZoomRatio(
    /* [in] */ ISize* activeArraySize,
    /* [in] */ ISize* cropSize,
    /* [out] */ ISizeF** sizef)
{
    VALIDATE_NOT_NULL(sizef);
    *sizef = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(activeArraySize, String("activeArraySize must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(cropSize, String("cropSize must not be null")))
    Int32 width;
    cropSize->GetWidth(&width);
    FAIL_RETURN(Preconditions::CheckArgumentPositive(width, String("cropSize.width must be positive")))
    Int32 height;
    cropSize->GetHeight(&height);
    FAIL_RETURN(Preconditions::CheckArgumentPositive(height, String("cropSize.height must be positive")))

    Int32 _width;
    activeArraySize->GetWidth(&_width);
    Int32 _height;
    activeArraySize->GetHeight(&_height);
    Float zoomRatioWidth = _width * 1.0f / width;
    Float zoomRatioHeight = _height * 1.0f / height;
    return CSizeF::New(zoomRatioWidth, zoomRatioHeight, sizef);
}

ECode ParameterUtils::ConvertScalerCropRegion(
    /* [in] */ IRect* activeArraySize,
    /* [in] */ IRect* cropRegion,
    /* [in] */ ISize* previewSize,
    /* [in] */ IParameters* params,
    /* [out] */ IParameterUtilsZoomData** outzd)
{
    VALIDATE_NOT_NULL(outzd);
    *outzd = NULL;

    AutoPtr<IRect> activeArraySizeOnly;
    Int32 width;
    activeArraySize->GetWidth(&width);
    Int32 height;
    activeArraySize->GetHeight(&height);
    CRect::New(/*left*/0, /*top*/0, width, height, (IRect**)&activeArraySizeOnly);

    AutoPtr<IRect> userCropRegion = cropRegion;

    if (userCropRegion == NULL) {
        userCropRegion = activeArraySizeOnly;
    }

    if (VERBOSE) {
        String str;
        IObject::Probe(userCropRegion)->ToString(&str);
        Logger::V(TAG, "convertScalerCropRegion - user crop region was %s", str.string());
    }

    AutoPtr<IRect> reportedCropRegion;
    CRect::New((IRect**)&reportedCropRegion);
    AutoPtr<IRect> previewCropRegion;
    CRect::New((IRect**)&previewCropRegion);
    Int32 zoomIdx;
    ParameterUtils::GetClosestAvailableZoomCrop(params, activeArraySizeOnly,
            previewSize, userCropRegion,
            /*out*/reportedCropRegion, /*out*/previewCropRegion, &zoomIdx);

    if (VERBOSE) {
        String str;
        IObject::Probe(reportedCropRegion)->ToString(&str);
        String str2;
        IObject::Probe(previewCropRegion)->ToString(&str2);
        Logger::V(TAG, "convertScalerCropRegion - zoom calculated to:"
                "zoomIndex = %d, reported crop region = %s, preview crop region"
                "= %s", zoomIdx, str.string(), str2.string());
    }

    return CZoomData::New(zoomIdx, previewCropRegion, reportedCropRegion, outzd);
}

ECode ParameterUtils::ConvertMeteringRectangleToLegacy(
    /* [in] */ IRect* activeArray,
    /* [in] */ IMeteringRectangle* meteringRect,
    /* [in] */ IParameterUtilsZoomData* zoomData,
    /* [out] */ IParameterUtilsMeteringData** outmd)
{
    VALIDATE_NOT_NULL(outmd);
    *outmd = NULL;

    AutoPtr<IRect> previewCrop;
    zoomData->GetPreviewCrop((IRect**)&previewCrop);

    Int32 width;
    previewCrop->GetWidth(&width);
    Float scaleW = (IParameterUtils::NORMALIZED_RECTANGLE_MAX -
            IParameterUtils::NORMALIZED_RECTANGLE_MIN) * 1.0f / width;
    Int32 height;
    previewCrop->GetHeight(&height);
    Float scaleH = (IParameterUtils::NORMALIZED_RECTANGLE_MAX -
            IParameterUtils::NORMALIZED_RECTANGLE_MIN) * 1.0f / height;

    AutoPtr<IMatrix> transform;
    CMatrix::New((IMatrix**)&transform);
    // Move the preview crop so that top,left is at (0,0), otherwise after scaling
    // the corner bounds will be outside of [-1000, 1000]
    Int32 left;
    previewCrop->GetLeft(&left);
    Int32 top;
    previewCrop->GetTop(&top);
    transform->SetTranslate(-left, -top);
    // Scale into [0, 2000] range about the center of the preview
    Boolean result;
    transform->PostScale(scaleW, scaleH, &result);
    // Move so that top left of a typical rect is at [-1000, -1000]
    transform->PostTranslate(/*dx*/IParameterUtils::NORMALIZED_RECTANGLE_MIN,
            /*dy*/IParameterUtils::NORMALIZED_RECTANGLE_MIN,  &result);

    /*
     * Calculate the preview metering region (effective), and the camera1 api
     * normalized metering region.
     */
    AutoPtr<IRect> rect;
    meteringRect->GetRect((IRect**)&rect);
    AutoPtr<IRect> normalizedRegionUnbounded;
    ParamsUtils::MapRect(transform, rect, (IRect**)&normalizedRegionUnbounded);

    /*
     * Try to intersect normalized area with [-1000, 1000] rectangle; otherwise
     * it's completely out of range
     */
    AutoPtr<IRect> normalizedIntersected;
    CRect::New(normalizedRegionUnbounded, (IRect**)&normalizedIntersected);

    AutoPtr<ICameraArea> meteringArea;
    normalizedIntersected->Intersect(NORMALIZED_RECTANGLE_DEFAULT, &result);
    if (!result) {
        Logger::W(TAG,
                "convertMeteringRectangleToLegacy - metering rectangle too small, "
                "no metering will be done");
        normalizedIntersected->Set(RECTANGLE_EMPTY);
        meteringArea = new HardwareCamera::Area(RECTANGLE_EMPTY,
                IMeteringRectangle::METERING_WEIGHT_DONT_CARE);
    }
    else {
        Int32 value;
        meteringRect->GetMeteringWeight(&value);
        meteringArea = new HardwareCamera::Area(normalizedIntersected, value);
    }

    /*
     * Calculate effective preview metering region
     */
    AutoPtr<IRect> previewMetering;
    meteringRect->GetRect((IRect**)&previewMetering);
    previewMetering->Intersect(previewCrop, &result);
    if (!result) {
        previewMetering->Set(RECTANGLE_EMPTY);
    }

    /*
     * Calculate effective reported metering region
     * - Transform the calculated metering area back into active array space
     * - Clip it to be a subset of the reported crop region
     */
    AutoPtr<IRect> reportedMetering;
    {
        Int32 value;
        meteringRect->GetMeteringWeight(&value);
        AutoPtr<ICameraArea> normalizedAreaUnbounded =
                new HardwareCamera::Area(normalizedRegionUnbounded, value);
        AutoPtr<IParameterUtilsWeightedRectangle> reportedMeteringRect;
        ConvertCameraAreaToActiveArrayRectangle(activeArray, zoomData, normalizedAreaUnbounded,
                /*usePreviewCrop*/FALSE, (IParameterUtilsWeightedRectangle**)&reportedMeteringRect);
        reportedMeteringRect->GetRect((IRect**)&reportedMetering);
    }

    if (VERBOSE) {
        String str;
        IObject::Probe(activeArray)->ToString(&str);
        String str2;
        IObject::Probe(meteringRect)->ToString(&str2);
        String str3;
        IObject::Probe(previewCrop)->ToString(&str3);
        String str4;
        IObject::Probe(previewMetering)->ToString(&str4);
        String str5;
        IObject::Probe(reportedMetering)->ToString(&str5);
        String str6;
        IObject::Probe(normalizedRegionUnbounded)->ToString(&str6);
        String tmp;
        StringFromArea(meteringArea, &tmp);
        Logger::V(TAG,
                "convertMeteringRectangleToLegacy - activeArray = %s, meteringRect = %s, "
                "previewCrop = %s, meteringArea = %s, previewMetering = %s, "
                "reportedMetering = %s, normalizedRegionUnbounded = %s",
                str.string(), str2.string(),
                str3.string(), tmp.string(), str4.string(),
                str5.string(), str6.string());
    }

    return CMeteringData::New(meteringArea, previewMetering, reportedMetering, outmd);
}

ECode ParameterUtils::ConvertCameraAreaToActiveArrayRectangle(
    /* [in] */ IRect* activeArray,
    /* [in] */ IParameterUtilsZoomData* zoomData,
    /* [in] */ ICameraArea* area,
    /* [out] */ IParameterUtilsWeightedRectangle** outwr)
{
    VALIDATE_NOT_NULL(outwr);

    return ConvertCameraAreaToActiveArrayRectangle(activeArray, zoomData, area,
                /*usePreviewCrop*/TRUE, outwr);
}

ECode ParameterUtils::ConvertFaceFromLegacy(
    /* [in] */ ICameraFace* face,
    /* [in] */ IRect* activeArray,
    /* [in] */ IParameterUtilsZoomData* zoomData,
    /* [out] */ IFace** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(face, String("face must not be null")))

    AutoPtr<IFace> api2Face;

    AutoPtr<IRect> rect;
    face->GetRect((IRect**)&rect);
    AutoPtr<ICameraArea> fakeArea = new HardwareCamera::Area(rect, /*weight*/1);

    AutoPtr<IParameterUtilsWeightedRectangle> faceRect;
    ConvertCameraAreaToActiveArrayRectangle(activeArray, zoomData,
            fakeArea, (IParameterUtilsWeightedRectangle**)&faceRect);

    AutoPtr<IPoint> leftEye;
    face->GetLeftEye((IPoint**)&leftEye);
    AutoPtr<IPoint> rightEye;
    face->GetRightEye((IPoint**)&rightEye);
    AutoPtr<IPoint> mouth;
    face->GetMouth((IPoint**)&mouth);
    if (leftEye != NULL && rightEye != NULL && mouth != NULL) {
        leftEye = ConvertCameraPointToActiveArrayPoint(activeArray, zoomData,
                leftEye, /*usePreviewCrop*/TRUE);
        rightEye = ConvertCameraPointToActiveArrayPoint(activeArray, zoomData,
                leftEye, /*usePreviewCrop*/TRUE);
        mouth = ConvertCameraPointToActiveArrayPoint(activeArray, zoomData,
                leftEye, /*usePreviewCrop*/TRUE);

        Int32 id;
        face->GetId(&id);
        faceRect->ToFace(id, leftEye, rightEye, mouth, (IFace**)&api2Face);
    }
    else {
        faceRect->ToFace((IFace**)&api2Face);
    }

    *outface = api2Face;
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

AutoPtr<IPoint> ParameterUtils::ConvertCameraPointToActiveArrayPoint(
    /* [in] */ IRect* activeArray,
    /* [in] */ IParameterUtilsZoomData* zoomData,
    /* [in] */ IPoint* point,
    /* [in] */ Boolean usePreviewCrop)
{
    Int32 x;
    point->GetX(&x);
    Int32 y;
    point->GetY(&y);
    AutoPtr<IRect> pointedRect;
    CRect::New(x, y, x, y, (IRect**)&pointedRect);
    AutoPtr<ICameraArea> pointedArea = new HardwareCamera::Area(pointedRect, /*weight*/1);

    AutoPtr<IParameterUtilsWeightedRectangle> adjustedRect;
    ConvertCameraAreaToActiveArrayRectangle(activeArray, zoomData, pointedArea,
            usePreviewCrop, (IParameterUtilsWeightedRectangle**)&adjustedRect);

    AutoPtr<IPoint> transformedPoint;
    AutoPtr<IRect> rect;
    adjustedRect->GetRect((IRect**)&rect);
    Int32 left;
    rect->GetLeft(&left);
    Int32 top;
    rect->GetTop(&top);
    CPoint::New(left, top, (IPoint**)&transformedPoint);
    return transformedPoint;
}

ECode ParameterUtils::ConvertCameraAreaToActiveArrayRectangle(
    /* [in] */ IRect* activeArray,
    /* [in] */ IParameterUtilsZoomData* zoomData,
    /* [in] */ ICameraArea* area,
    /* [in] */ Boolean usePreviewCrop,
    /* [out] */ IParameterUtilsWeightedRectangle** rectangle)
{
    VALIDATE_NOT_NULL(rectangle);
    *rectangle = NULL;

    AutoPtr<IRect> previewCrop;
    zoomData->GetPreviewCrop((IRect**)&previewCrop);
    AutoPtr<IRect> reportedCrop;
    zoomData->GetReportedCrop((IRect**)&reportedCrop);

    Int32 width;
    previewCrop->GetWidth(&width);
    Float scaleW = width * 1.0f /
            (IParameterUtils::NORMALIZED_RECTANGLE_MAX - IParameterUtils::NORMALIZED_RECTANGLE_MIN);
    Int32 height;
    previewCrop->GetHeight(&height);
    Float scaleH = height * 1.0f /
            (IParameterUtils::NORMALIZED_RECTANGLE_MAX - IParameterUtils::NORMALIZED_RECTANGLE_MIN);

    /*
     * Calculate the reported metering region from the non-intersected normalized region
     * by scaling and translating back into active array-relative coordinates.
     */
    AutoPtr<IMatrix> transform;
    CMatrix::New((IMatrix**)&transform);

    // Move top left from (-1000, -1000) to (0, 0)
    transform->SetTranslate(/*dx*/IParameterUtils::NORMALIZED_RECTANGLE_MAX,
            /*dy*/IParameterUtils::NORMALIZED_RECTANGLE_MAX);

    // Scale from [0, 2000] back into the preview rectangle
    Boolean result;
    transform->PostScale(scaleW, scaleH, &result);

    // Move the rect so that the [-1000,-1000] point ends up at the preview [left, top]
    Int32 left;
    previewCrop->GetLeft(&left);
    Int32 top;
    previewCrop->GetTop(&top);
    transform->PostTranslate(left, top, &result);

    AutoPtr<IRect> cropToIntersectAgainst = usePreviewCrop ? previewCrop : reportedCrop;

    // Now apply the transformation backwards to get the reported metering region
    AutoPtr<IRect> reportedMetering;
    AutoPtr<IRect> rect;
    area->GetRect((IRect**)&rect);
    ParamsUtils::MapRect(transform, rect, (IRect**)&reportedMetering);
    // Intersect it with the crop region, to avoid reporting out-of-bounds
    // metering regions
    reportedMetering->Intersect(cropToIntersectAgainst, &result);
    if (!result) {
        reportedMetering->Set(RECTANGLE_EMPTY);
    }

    Int32 weight;
    area->GetWeight(&weight);
    if (weight < IMeteringRectangle::METERING_WEIGHT_MIN) {
        String str;
        StringFromArea(area, &str);
        Logger::W(TAG,
                "convertCameraAreaToMeteringRectangle - rectangle %s has"
                "too small weight, clip to 0", str.string());
        weight = 0; //do not use, why set 0?
    }

    area->GetWeight(&weight);
    return CWeightedRectangle::New(reportedMetering, weight, rectangle);
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
