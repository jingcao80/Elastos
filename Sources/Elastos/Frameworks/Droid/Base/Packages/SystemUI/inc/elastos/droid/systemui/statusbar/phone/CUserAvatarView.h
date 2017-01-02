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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CUSERAVATARVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CUSERAVATARVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CUserAvatarView.h"
#include <elastos/droid/view/View.h>

using Elastos::Droid::View::View;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * A view that displays a user image cropped to a circle with a frame.
 */
CarClass(CUserAvatarView)
    , public Elastos::Droid::View::View
    , public IUserAvatarView
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CUserAvatarView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SetBitmap(
        /* [in] */ IBitmap* bitmap);

    CARAPI SetFrameColor(
        /* [in] */ Int32 frameColor);

    CARAPI SetActiveFrameColor(
        /* [in] */ Int32 activeFrameColor);

    CARAPI SetFrameWidth(
        /* [in] */ Float frameWidth);

    CARAPI SetFramePadding(
        /* [in] */ Float framePadding);

    CARAPI ConfigureBounds();

    CARAPI SetDrawable(
        /* [in] */ IDrawable* d);

    // @Override
    CARAPI InvalidateDrawable(
        /* [in] */ IDrawable* dr);

protected:
    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    // @Override
    CARAPI_(Boolean) VerifyDrawable(
        /* [in] */ IDrawable* who);

    // @Override
    CARAPI DrawableStateChanged();

private:
    Int32 mActiveFrameColor;
    Int32 mFrameColor;
    Float mFrameWidth;
    Float mFramePadding;
    AutoPtr<IBitmap> mBitmap;
    AutoPtr<IDrawable> mDrawable;

    AutoPtr<IPaint> mFramePaint;
    AutoPtr<IPaint> mBitmapPaint;
    AutoPtr<IMatrix> mDrawMatrix;

    Float mScale;
};

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CUSERAVATARVIEW_H__
