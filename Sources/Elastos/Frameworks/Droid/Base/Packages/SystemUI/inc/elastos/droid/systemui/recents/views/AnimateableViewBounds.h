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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_ANIMATEABLEVIEWBOUNDS_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_ANIMATEABLEVIEWBOUNDS_H__

#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include "Elastos.Droid.Animation.h"
#include <elastos/droid/view/ViewOutlineProvider.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewOutlineProvider;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/* An outline provider that has a clip and outline that can be animated. */
class AnimateableViewBounds
    : public ViewOutlineProvider
    , public IAnimateableViewBounds
{
public:
    CAR_INTERFACE_DECL()

    AnimateableViewBounds();

    CARAPI constructor(
        /* [in] */ ITaskView* source,
        /* [in] */ Int32 cornerRadius);

    // @Override
    CARAPI GetOutline(
        /* [in] */ IView* view,
        /* [in] */ IOutline* outline);

    /** Sets the view outline alpha. */
    CARAPI_(void) SetAlpha(
        /* [in] */ Float alpha);

    /** Animates the top clip. */
    CARAPI_(void) AnimateClipTop(
        /* [in] */ Int32 top,
        /* [in] */ Int32 duration,
        /* [in] */ IAnimatorUpdateListener* updateListener);

    /** Sets the top clip. */
    CARAPI SetClipTop(
        /* [in] */ Int32 top);

    CARAPI GetClipTop(
        /* [out] */ Int32* top);

    /** Returns the top clip. */
    CARAPI_(Int32) GetClipTop();

    /** Animates the right clip. */
    CARAPI_(void) AnimateClipRight(
        /* [in] */ Int32 right,
        /* [in] */ Int32 duration);

    /** Sets the right clip. */
    CARAPI SetClipRight(
        /* [in] */ Int32 right);

    CARAPI GetClipRight(
        /* [out] */ Int32* right);

    /** Returns the right clip. */
    CARAPI_(Int32) GetClipRight();

    /** Animates the bottom clip. */
    CARAPI_(void) AnimateClipBottom(
        /* [in] */ Int32 bottom,
        /* [in] */ Int32 duration);

    /** Sets the bottom clip. */
    CARAPI SetClipBottom(
        /* [in] */ Int32 bottom);

    CARAPI GetClipBottom(
        /* [in] */ Int32* bottom);

    /** Returns the bottom clip. */
    CARAPI_(Int32) GetClipBottom();

    /** Sets the outline bottom clip. */
    CARAPI_(void) SetOutlineClipBottom(
        /* [in] */ Int32 bottom);

    /** Gets the outline bottom clip. */
    CARAPI_(Int32) GetOutlineClipBottom();

private:
    CARAPI_(void) UpdateClipBounds();

public:
    AutoPtr<RecentsConfiguration> mConfig;

    IView* mSourceView; // TaskView, TaskView has this's reference
    AutoPtr<IRect> mTmpRect;
    AutoPtr<IRect> mClipRect;
    AutoPtr<IRect> mClipBounds;
    AutoPtr<IRect> mOutlineClipRect;
    Int32 mCornerRadius;
    Float mAlpha;
    Float mMinAlpha;

    AutoPtr<IAnimator> mClipTopAnimator; //IObjectAnimator
    AutoPtr<IAnimator> mClipRightAnimator; //IObjectAnimator
    AutoPtr<IAnimator> mClipBottomAnimator; //IObjectAnimator
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_ANIMATEABLEVIEWBOUNDS_H__
