
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONOVERFLOWICONSVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONOVERFLOWICONSVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CNotificationOverflowIconsView.h"
#include "elastos/droid/systemui/statusbar/phone/IconMerger.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::App::INotification;
using Elastos::Droid::Internal::Utility::INotificationColorUtil;
using Elastos::Droid::SystemUI::StatusBar::Phone::IconMerger;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * A view to display all the overflowing icons on Keyguard.
 */
CarClass(CNotificationOverflowIconsView)
    , public IconMerger
    , public INotificationOverflowIconsView
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CNotificationOverflowIconsView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetMoreText(
        /* [in] */ ITextView* moreText);

    CARAPI AddNotification(
        /* [in] */ INotificationDataEntry* notification);

protected:
    // @Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(void) ApplyColor(
        /* [in] */ INotification* notification,
        /* [in] */ IStatusBarIconView* view);

    CARAPI_(void) UpdateMoreText();

private:
    AutoPtr<ITextView> mMoreText;
    Int32 mTintColor;
    Int32 mIconSize;
    AutoPtr<INotificationColorUtil> mNotificationColorUtil;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONOVERFLOWICONSVIEW_H__
