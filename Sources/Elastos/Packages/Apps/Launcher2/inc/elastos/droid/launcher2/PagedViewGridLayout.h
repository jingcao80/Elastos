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

#ifndef  __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWGRIDLAYOUT_H__
#define  __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWGRIDLAYOUT_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/widget/GridLayout.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Core.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::GridLayout;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * The grid based layout used strictly for the widget/wallpaper tab of the AppsCustomize pane
 */
class PagedViewGridLayout
    : public GridLayout
    , public IPagedViewGridLayout
    , public IPage
{
public:
    class PagedViewGridLayoutLayoutParams
        : public FrameLayout::LayoutParams
    {
    public:
        PagedViewGridLayoutLayoutParams(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);
    };

public:
    CAR_INTERFACE_DECL();

    TO_STRING_IMPL("PagedViewGridLayout")

    PagedViewGridLayout(
        /* [in] */ IContext* context,
        /* [in] */ Int32 cellCountX,
        /* [in] */ Int32 cellCountY);

    CARAPI GetCellCountX(
        /* [out] */ Int32* x);

    CARAPI GetCellCountY(
        /* [out] */ Int32* y);

    /**
     * Clears all the key listeners for the individual widgets.
     */
    CARAPI ResetChildrenOnKeyListeners();

    CARAPI SetOnLayoutListener(
        /* [in] */ IRunnable* r);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI RemoveAllViewsOnPage();

    //@Override
    CARAPI RemoveViewOnPageAt(
        /* [in] */ Int32 index);

    //@Override
    CARAPI GetPageChildCount(
        /* [out] */ Int32* count);

    //@Override
    CARAPI GetChildOnPageAt(
        /* [in] */ Int32 i,
        /* [out] */ IView** view);

    //@Override
    CARAPI IndexOfChildOnPage(
        /* [in] */ IView* v,
        /* [out] */ Int32* index);

protected:
    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    CARAPI OnDetachedFromWindow();

    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

protected:
    static const String TAG;

private:
    Int32 mCellCountX;
    Int32 mCellCountY;
    AutoPtr<IRunnable> mOnLayoutListener;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWGRIDLAYOUT_H__