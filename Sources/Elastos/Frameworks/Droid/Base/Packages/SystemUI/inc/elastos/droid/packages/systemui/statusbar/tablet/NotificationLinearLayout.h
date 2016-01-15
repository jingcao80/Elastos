#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_NOTIFICATIONLINEARLAYOUT_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_NOTIFICATIONLINEARLAYOUT_H__

#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


class NotificationLinearLayout : public LinearLayout
{
public:
    NotificationLinearLayout();

    NotificationLinearLayout(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    NotificationLinearLayout(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    virtual CARAPI OnFinishInflate();

    virtual void OnDraw(
        /* [in] */ ICanvas* canvas);

protected:
    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI InitImpl(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

private:
    static const String TAG;

    AutoPtr<IDrawable> mItemGlow;
    Int32 mInsetLeft;
    AutoPtr<IRect> mTmp;// = new Rect();
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_NOTIFICATIONLINEARLAYOUT_H__
