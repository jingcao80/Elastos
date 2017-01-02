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

#ifndef __ELASTOS_DROID_DIALER_LIST_TILEINTERACTIONTEASERVIEW_H__
#define __ELASTOS_DROID_DIALER_LIST_TILEINTERACTIONTEASERVIEW_H__

#include "_Elastos.Droid.Dialer.h"
// #include "elastos/droid/dialer/list/ShortcutCardsAdapter.h"
#include <elastos/droid/widget/FrameLayout.h>
#include <elastos/core/Runnable.h>
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Content::IContext;
using Elastos::Core::Runnable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

/**
 * A teaser to introduce people to the contact photo check boxes
 */
class TileInteractionTeaserView
    : public FrameLayout
    , public ITileInteractionTeaserView
{
private:
    class DismissClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        DismissClickListener(
            /* [in] */ TileInteractionTeaserView* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        TileInteractionTeaserView* mHost;
    };

    class ArrowRunnable
        : public Runnable
    {
    public:
        ArrowRunnable(
            /* [in] */ TileInteractionTeaserView* host,
            /* [in] */ ITextView* text,
            /* [in] */ IImageView* arrow);

        // @Override
        CARAPI Run();

    private:
        TileInteractionTeaserView* mHost;
        AutoPtr<ITextView> mText;
        AutoPtr<IImageView> mArrow;
    };

    class HeightAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        HeightAnimatorUpdateListener(
            /* [in] */ TileInteractionTeaserView* host);

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        TileInteractionTeaserView* mHost;
    };

    class HeightAnimatorListener
        : public Object
        , public IAnimatorListener
    {
    public:
        CAR_INTERFACE_DECL();

        HeightAnimatorListener(
            /* [in] */ TileInteractionTeaserView* host);

        // @Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animator);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animator);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animator);

        // @Override
        CARAPI OnAnimationRepeat(
            /* [in] */ IAnimator* animator);

    private:
        TileInteractionTeaserView* mHost;
    };

public:
    TileInteractionTeaserView();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(Boolean) GetShouldDisplayInList();

    // CARAPI_(void) SetAdapter(
    //     /* [in] */ ShortcutCardsAdapter* adapter);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    // @Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

private:
    CARAPI_(void) StartDestroyAnimation();

    CARAPI_(void) SetDismissed();

private:
    static Int32 sShrinkAnimationDuration;

    static const String KEY_TILE_INTERACTION_TEASER_SHOWN; // =
            // "key_tile_interaction_teaser_shown";

    Boolean mNeedLayout;
    Int32 mTextTop;
    Int32 mAnimatedHeight;

    // AutoPtr<ShortcutCardsAdapter> mAdapter;

    friend class DismissClickListener;
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_TILEINTERACTIONTEASERVIEW_H__
