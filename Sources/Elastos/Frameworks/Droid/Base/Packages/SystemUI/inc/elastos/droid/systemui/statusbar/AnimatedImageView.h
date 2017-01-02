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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_ANIMATEDIMAGEVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_ANIMATEDIMAGEVIEW_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/droid/widget/ImageView.h>

using Elastos::Droid::Graphics::Drawable::IAnimationDrawable;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ImageView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

class AnimatedImageView
    : public ImageView
    , public IAnimatedImageView
{
public:
    CAR_INTERFACE_DECL()

    AnimatedImageView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    virtual CARAPI SetImageResource(
        /* [in] */ Int32 resId);

    virtual CARAPI SetImageDrawable(
        /* [in] */ IDrawable* drawable);

    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

protected:
    virtual CARAPI OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 vis);

private:
    CARAPI_(void) UpdateAnim();

private:
    AutoPtr<IAnimationDrawable> mAnim;
    Boolean mAttached;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_ANIMATEDIMAGEVIEW_H__
