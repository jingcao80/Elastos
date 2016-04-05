
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