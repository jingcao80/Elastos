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

#ifndef  __ELASTOS_DROID_LAUNCHER2_BUBBLETEXTVIEW_H__
#define  __ELASTOS_DROID_LAUNCHER2_BUBBLETEXTVIEW_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/HolographicOutlineHelper.h"
#include "elastos/droid/launcher2/CheckLongPressHelper.h"
#include "elastos/droid/widget/TextView.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * TextView that draws a bubble behind the text. We cannot use a LineBackgroundSpan
 * because we want to make the bubble taller than the text and TextView's clip is
 * too aggressive.
 */
class BubbleTextView
    : public TextView
    , public IBubbleTextView
{
public:
    CAR_INTERFACE_DECL();

    BubbleTextView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI ApplyFromShortcutInfo(
        /* [in] */ IShortcutInfo* info,
        /* [in] */ IIconCache* iconCache);

    //@Override
    CARAPI SetTag(
        /* [in] */ IInterface* tag);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI SetStayPressed(
        /* [in] */ Boolean stayPressed);

    CARAPI SetCellLayoutPressedOrFocusedIcon();

    CARAPI ClearPressedOrFocusedBackground();

    CARAPI GetPressedOrFocusedBackground(
        /* [out] */ IBitmap** map);

    CARAPI GetPressedOrFocusedBackgroundPadding(
        /* [out] */ Int32* res);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI CancelLongPress();

protected:
    //@Override
    CARAPI_(Boolean) SetFrame(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    //@Override
    CARAPI_(Boolean) VerifyDrawable(
        /* [in] */ IDrawable* who);

    //@Override
    CARAPI DrawableStateChanged();

    //@Override
    CARAPI OnAttachedToWindow();

    //@Override
    CARAPI OnDetachedFromWindow();

    //@Override
    CARAPI_(Boolean) OnSetAlpha(
        /* [in] */ Int32 alpha);

private:
    CARAPI Init();

    /**
     * Draw this BubbleTextView into the given Canvas.
     *
     * @param destCanvas the canvas to draw on
     * @param padding the horizontal and vertical padding to use when drawing
     */
    CARAPI_(void) DrawWithPadding(
        /* [in] */ ICanvas* destCanvas,
        /* [in] */ Int32 padding);

    /**
     * Returns a new bitmap to be used as the object outline, e.g. to visualize the drop location.
     * Responsibility for the bitmap is transferred to the caller.
     */
    CARAPI_(AutoPtr<IBitmap>) CreateGlowingOutline(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Int32 outlineColor,
        /* [in] */ Int32 glowColor);

private:
    Int32 mPrevAlpha;

    Boolean mDidInvalidateForPressedState;
    AutoPtr<IBitmap> mPressedOrFocusedBackground;
    Int32 mFocusedOutlineColor;
    Int32 mFocusedGlowColor;
    Int32 mPressedOutlineColor;
    Int32 mPressedGlowColor;

    Boolean mBackgroundSizeChanged;
    AutoPtr<IDrawable> mBackground;

    Boolean mStayPressed;
    AutoPtr<CheckLongPressHelper> mLongPressHelper;

    AutoPtr<HolographicOutlineHelper> mOutlineHelper;
    AutoPtr<ICanvas> mTempCanvas;
    AutoPtr<IRect> mTempRect;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_BUBBLETEXTVIEW_H__