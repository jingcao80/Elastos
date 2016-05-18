
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_CDATAUSAGEGRAPH_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_CDATAUSAGEGRAPH_H__

#include "_Elastos_Droid_SystemUI_Qs_CDataUsageGraph.h"
#include <elastos/droid/view/View.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

CarClass(CDataUsageGraph)
    , public Elastos::Droid::View::View
    , public IDataUsageGraph
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDataUsageGraph();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetLevels(
        /* [in] */ Int64 limitLevel,
        /* [in] */ Int64 warningLevel,
        /* [in] */ Int64 usageLevel);

protected:
    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    Int32 mTrackColor;
    Int32 mUsageColor;
    Int32 mOverlimitColor;
    Int32 mWarningColor;
    Int32 mMarkerWidth;
    AutoPtr<IRectF> mTmpRect;
    AutoPtr<IPaint> mTmpPaint;

    Int64 mLimitLevel;
    Int64 mWarningLevel;
    Int64 mUsageLevel;
    Int64 mMaxLevel;
};

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_CDATAUSAGEGRAPH_H__
