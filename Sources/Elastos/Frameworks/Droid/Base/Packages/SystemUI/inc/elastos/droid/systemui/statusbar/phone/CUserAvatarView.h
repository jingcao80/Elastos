
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
