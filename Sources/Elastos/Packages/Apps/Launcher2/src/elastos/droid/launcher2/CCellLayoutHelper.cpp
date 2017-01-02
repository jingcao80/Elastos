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

#include "elastos/droid/launcher2/CCellLayoutHelper.h"
#include "elastos/droid/launcher2/CellLayout.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_SINGLETON_IMPL(CCellLayoutHelper)

CAR_INTERFACE_IMPL(CCellLayoutHelper, Singleton, ICellLayoutHelper)

ECode CCellLayoutHelper::WidthInPortrait(
    /* [in] */ IResources* r,
    /* [in] */ Int32 numCells,
    /* [out] */ Int32* outWidth)
{
    VALIDATE_NOT_NULL(outWidth);

    return CellLayout::WidthInPortrait(r, numCells, outWidth);
}

ECode CCellLayoutHelper::HeightInLandscape(
    /* [in] */ IResources* r,
    /* [in] */ Int32 numCells,
    /* [out] */ Int32* outHeight)
{
    VALIDATE_NOT_NULL(outHeight);

    return CellLayout::HeightInLandscape(r, numCells, outHeight);
}

ECode CCellLayoutHelper::GetMetrics(
    /* [in] */ IRect* metrics,
    /* [in] */ IResources* res,
    /* [in] */ Int32 measureWidth,
    /* [in] */ Int32 measureHeight,
    /* [in] */ Int32 countX,
    /* [in] */ Int32 countY,
    /* [in] */ Int32 orientation)
{
    return CellLayout::GetMetrics(metrics, res, measureWidth,
            measureHeight, countX, countY, orientation);
}

ECode CCellLayoutHelper::RectToCell(
    /* [in] */ IResources* resources,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ ArrayOf<Int32>* result,
    /* [out, callee] */ ArrayOf<Int32>** outArray)
{
    VALIDATE_NOT_NULL(outArray);

    return CellLayout::RectToCell(resources, width, height, result, outArray);
}

ECode CCellLayoutHelper::FindVacantCell(
    /* [in] */ ArrayOf<Int32>* vacant,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ Int32 xCount,
    /* [in] */ Int32 yCount,
    /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return CellLayout::FindVacantCell(vacant, spanX, spanY, xCount,
            yCount, occupied, result);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos