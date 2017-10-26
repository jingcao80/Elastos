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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONBACKGROUNDVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONBACKGROUNDVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CNotificationBackgroundView.h"
#include <elastos/droid/view/View.h>

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * A view that can be used for both the dimmed and normal background of an notification.
 */
CarClass(CNotificationBackgroundView)
    , public Elastos::Droid::View::View
    , public INotificationBackgroundView
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CNotificationBackgroundView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI DrawableHotspotChanged(
        /* [in] */ Float x,
        /* [in] */ Float y);

    /**
     * Sets a background drawable. As we need to change our bounds independently of layout, we need
     * the notion of a background independently of the regular View background..
     */
    CARAPI SetCustomBackground(
        /* [in] */ IDrawable* background);

    CARAPI SetCustomBackground(
        /* [in] */ Int32 drawableResId);

    CARAPI SetTint(
        /* [in] */ Int32 tintColor);

    CARAPI SetActualHeight(
        /* [in] */ Int32 actualHeight);

    CARAPI GetActualHeight(
        /* [out] */ Int32* height);

    CARAPI SetClipTopAmount(
        /* [in] */ Int32 clipTopAmount);

    // @Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* has);

    CARAPI SetState(
        /* [in] */ ArrayOf<Int32>* drawableState);

    CARAPI SetRippleColor(
        /* [in] */ Int32 color);

protected:
    // @Override
    CARAPI_(Boolean) VerifyDrawable(
        /* [in] */ IDrawable* who);

    // @Override
    CARAPI DrawableStateChanged();

private:
    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    using View::Draw;

    CARAPI_(void) Draw(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IDrawable* drawable);

    CARAPI_(void) DrawableStateChanged(
        /* [in] */ IDrawable* d);

private:
    AutoPtr<IDrawable> mBackground;
    Int32 mClipTopAmount;
    Int32 mActualHeight;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONBACKGROUNDVIEW_H__
