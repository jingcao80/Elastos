#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STATUSBARICONVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STATUSBARICONVIEW_H__

#include "elastos/droid/systemui/statusbar/AnimatedImageView.h"

using Elastos::Droid::App::INotification;
using Elastos::Droid::StatusBar::IStatusBarIcon;
using Elastos::Droid::Os::IUserHandle;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

class StatusBarIconView : public AnimatedImageView
{
public:
    StatusBarIconView();

    StatusBarIconView(
        /* [in] */ IContext* context,
        /* [in] */ const String& slot,
        /* [in] */ INotification* notification);

    StatusBarIconView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    Boolean Set(
        /* [in] */ IStatusBarIcon* icon);

    /**
     * Returns the right icon to use for this item, respecting the iconId and
     * iconPackage (if set)
     *
     * @param context Context to use to get resources if iconPackage is not set
     * @return Drawable for this item, or null if the package or item could not
     *         be found
     */
    static AutoPtr<IDrawable> GetIcon(
        /* [in] */ IContext* context,
        /* [in] */ IStatusBarIcon* icon);

    AutoPtr<IStatusBarIcon> GetStatusBarIcon();

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    String ToString();

protected:
    virtual CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    void PlaceNumber();

    ECode Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    ECode Init(
        /* [in] */ IContext* context,
        /* [in] */ const String& slot,
        /* [in] */ INotification* notification);

private:
    ECode InitWithNotification(
        /* [in] */ IContext* context,
        /* [in] */ const String& slot,
        /* [in] */ INotification* notification);

    ECode InitPrivate(
        /* [in] */ IContext* context);

    static Boolean StrEQ(
        /* [in] */ const String& a,
        /* [in] */ const String& b);

    AutoPtr<IDrawable> GetIcon(
        /* [in] */ IStatusBarIcon* icon);

    void SetContentDescriptionByNotification(
        /* [in] */ INotification* notification);

private:
    static const String TAG;
    static const Boolean DBG;

    AutoPtr<IStatusBarIcon> mIcon;
    String mSlot;
    AutoPtr<IDrawable> mNumberBackground;
    AutoPtr<IPaint> mNumberPain;
    Int32 mNumberX;
    Int32 mNumberY;
    String mNumberText;
    AutoPtr<INotification> mNotification;
};


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_STATUSBARICONVIEW_H__
