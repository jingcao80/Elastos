#ifndef __ELASTOS_DROID_LAUNCHER2_CCELLLAYOUTHELPER_H__
#define __ELASTOS_DROID_LAUNCHER2_CCELLLAYOUTHELPER_H__

#include "_Launcher2.h"
#include "_Elastos_Droid_Launcher2_CCellLayoutHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CCellLayoutHelper)
    , public Singleton
    , public ICellLayoutHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI WidthInPortrait(
        /* [in] */ IResources* r,
        /* [in] */ Int32 numCells,
        /* [out] */ Int32* outWidth);

    CARAPI HeightInLandscape(
        /* [in] */ IResources* r,
        /* [in] */ Int32 numCells,
        /* [out] */ Int32* outHeight);

    CARAPI GetMetrics(
        /* [in] */ IRect* metrics,
        /* [in] */ IResources* res,
        /* [in] */ Int32 measureWidth,
        /* [in] */ Int32 measureHeight,
        /* [in] */ Int32 countX,
        /* [in] */ Int32 countY,
        /* [in] */ Int32 orientation);

    CARAPI RectToCell(
        /* [in] */ IResources* resources,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ ArrayOf<Int32>* result,
        /* [out, callee] */ ArrayOf<Int32>** outArray);

    CARAPI FindVacantCell(
        /* [in] */ ArrayOf<Int32>* vacant,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ Int32 xCount,
        /* [in] */ Int32 yCount,
        /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied,
        /* [out] */ Boolean* result);
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CCELLLAYOUTHELPER_H__