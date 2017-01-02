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
