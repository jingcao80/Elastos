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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_EXPANDABLEOUTLINEVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_EXPANDABLEOUTLINEVIEW_H__

#include "elastos/droid/systemui/statusbar/ExpandableView.h"
#include "Elastos.Droid.Graphics.h"
#include <elastos/droid/view/ViewOutlineProvider.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::ViewOutlineProvider;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * Like {@link ExpandableView}, but setting an outline for the height and clipping.
 */
class ExpandableOutlineView
    : public ExpandableView
    , public IExpandableOutlineView
{
private:
    class ExpandableViewOutlineProvider: public ViewOutlineProvider
    {
    public:
        ExpandableViewOutlineProvider(
            /* [in] */ ExpandableOutlineView* host);

        // @Override
        CARAPI GetOutline(
            /* [in] */ IView* view,
            /* [in] */ IOutline* outline);

    private:
        ExpandableOutlineView* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ExpandableOutlineView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI SetActualHeight(
        /* [in] */ Int32 actualHeight,
        /* [in] */ Boolean notifyListeners);

    // @Override
    CARAPI SetClipTopAmount(
        /* [in] */ Int32 clipTopAmount);

protected:
    virtual CARAPI_(void) SetOutlineRect(
        /* [in] */ IRectF* rect);

    virtual CARAPI_(void) SetOutlineRect(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom);

private:
    AutoPtr<IRect> mOutlineRect;
    Boolean mCustomOutline;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_EXPANDABLEOUTLINEVIEW_H__
