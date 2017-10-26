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

#ifndef  __ELASTOS_DROID_LAUNCHER2_SHORTCUTANDWIDGETCONTAINER_H__
#define  __ELASTOS_DROID_LAUNCHER2_SHORTCUTANDWIDGETCONTAINER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/ViewGroup.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::App::IWallpaperManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewGroup;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class ShortcutAndWidgetContainer
    : public ViewGroup
    , public IShortcutAndWidgetContainer
{
public:
    CAR_INTERFACE_DECL();

    ShortcutAndWidgetContainer();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SetCellDimensions(
        /* [in] */ Int32 cellWidth,
        /* [in] */ Int32 cellHeight,
        /* [in] */ Int32 widthGap,
        /* [in] */ Int32 heightGap,
        /* [in] */ Int32 countX);

    CARAPI GetChildAt(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ IView** view);

    using ViewGroup::GetChildAt;

    CARAPI SetupLp(
        /* [in] */ ICellLayoutLayoutParams* lp);

    // Set whether or not to invert the layout horizontally if the layout is in RTL mode.
    CARAPI SetInvertIfRtl(
        /* [in] */ Boolean invert);

    using ViewGroup::MeasureChild;

    CARAPI MeasureChild(
        /* [in] */ IView* child);

    CARAPI IsLayoutRtl(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI ShouldDelayChildPressedState(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI RequestChildFocus(
        /* [in] */ IView* child,
        /* [in] */ IView* focused);

    //@Override
    CARAPI CancelLongPress();

protected:
    friend class CellLayout;

    //@Override
    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    //@Override
    CARAPI_(void) SetChildrenDrawingCacheEnabled(
        /* [in] */ Boolean enabled);

    //@Override
    CARAPI_(void) SetChildrenDrawnWithCacheEnabled(
        /* [in] */ Boolean enabled);

private:
    CARAPI_(Boolean) InvertLayoutHorizontally();

public:
    static const String TAG;

private:
    AutoPtr<IWallpaperManager> mWallpaperManager;

    Int32 mCellWidth;
    Int32 mCellHeight;

    Int32 mWidthGap;
    Int32 mHeightGap;

    Int32 mCountX;

    Boolean mInvertIfRtl;

    // These are temporary variables to prevent having to allocate a new object just to
    // return an (x, y) value from helper functions. Do NOT use them to maintain other state.
    AutoPtr<ArrayOf<Int32> > mTmpCellXY;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_SHORTCUTANDWIDGETCONTAINER_H__