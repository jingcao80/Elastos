
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_CCLOCK_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_CCLOCK_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CClock.h"
#include "Elastos.CoreLibrary.Text.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/widget/TextView.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::TextView;
using Elastos::Utility::ICalendar;
using Elastos::Text::ISimpleDateFormat;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class CClockIntentReceiver;

/**
 * Digital clock for the status bar.
 */
CarClass(CClock)
    , public TextView
    , public IClock
    , public IDemoMode
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CClock();

    CARAPI constructor(
        /* [in] */ IContext* pCtx);

    CARAPI constructor(
        /* [in] */ IContext* pCtx,
        /* [in] */ IAttributeSet * pAttrs);

    CARAPI constructor(
        /* [in] */ IContext* pContext,
        /* [in] */ IAttributeSet* pAttrs,
        /* [in] */ Int32 defStyle);

    // @Override
    CARAPI DispatchDemoCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle* args);

protected:
    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDetachedFromWindow();

private:
    CARAPI_(void) UpdateClock();

    CARAPI_(AutoPtr<ICharSequence>) GetSmallTime();

private:
    Boolean mAttached;
    AutoPtr<ICalendar> mCalendar;
    String mClockFormatString;
    AutoPtr<ISimpleDateFormat> mClockFormat;
    AutoPtr<ILocale> mLocale;

    static const Int32 AM_PM_STYLE_NORMAL;
    static const Int32 AM_PM_STYLE_SMALL;
    static const Int32 AM_PM_STYLE_GONE;

    Int32 mAmPmStyle;

    Boolean mDemoMode;
    AutoPtr<IBroadcastReceiver> mIntentReceiver;
    friend class CClockIntentReceiver;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_CCLOCK_H__
