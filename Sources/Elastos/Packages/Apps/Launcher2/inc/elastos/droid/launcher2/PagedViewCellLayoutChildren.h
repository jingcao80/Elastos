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

#ifndef  __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWCELLLAYOUTCHILDREN_H__
#define  __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWCELLLAYOUTCHILDREN_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/ViewGroup.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewGroup;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * An abstraction of the original CellLayout which supports laying out items
 * which span multiple cells into a grid-like layout.  Also supports dimming
 * to give a preview of its contents.
 */
class PagedViewCellLayoutChildren
    : public ViewGroup
    , public IPagedViewCellLayoutChildren
{
public:
    CAR_INTERFACE_DECL();

    TO_STRING_IMPL("PagedViewCellLayoutChildren")

    PagedViewCellLayoutChildren(
        /* [in] */ IContext* context);

    //@Override
    CARAPI CancelLongPress();

    CARAPI SetGap(
        /* [in] */ Int32 widthGap,
        /* [in] */ Int32 heightGap);

    CARAPI SetCellDimensions(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    //@Override
    CARAPI RequestChildFocus(
        /* [in] */ IView* child,
        /* [in] */ IView* focused);

    CARAPI EnableCenteredContent(
        /* [in] */ Boolean enabled);

protected:
    friend class PagedViewCellLayout;

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

protected:
    static const String TAG;

private:
    Boolean mCenterContent;

    Int32 mCellWidth;
    Int32 mCellHeight;
    Int32 mWidthGap;
    Int32 mHeightGap;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWCELLLAYOUTCHILDREN_H__