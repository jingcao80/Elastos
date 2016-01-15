#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_ANIMATEDIMAGEVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_ANIMATEDIMAGEVIEW_H__

#include "elastos/droid/widget/ImageView.h"

using Elastos::Droid::Graphics::Drawable::IAnimationDrawable;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ImageView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

class AnimatedImageView : public ImageView
{
public:
    AnimatedImageView();

    AnimatedImageView(
        /* [in] */ IContext* context);

    AnimatedImageView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    virtual CARAPI SetImageResource(
        /* [in] */ Int32 resId);

    virtual CARAPI SetImageDrawable(
        /* [in] */ IDrawable* drawable);

    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

protected:
    virtual CARAPI_(void) OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 vis);

private:
    CARAPI_(void) UpdateAnim();

private:
    AutoPtr<IAnimationDrawable> mAnim;
    Boolean mAttached;
};


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_ANIMATEDIMAGEVIEW_H__
