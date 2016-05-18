#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CDATEVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CDATEVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CDateView.h"
#include "Elastos.CoreLibrary.Text.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/widget/TextView.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Widget::TextView;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::IDate;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CarClass(CDateView)
    , public TextView
    , public IDateView
{
private:
    class DateViewBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        DateViewBroadcastReceiver(
            /* [in] */ CDateView* bar);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* str)
        {
            *str = String("CDateView.DateViewBroadcastReceiver");
            return NOERROR;
        }

    private:
        CDateView* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CDateView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDetachedFromWindow();

    CARAPI_(void) UpdateClock();

private:
    static const String TAG;

    AutoPtr<IDate> mCurrentTime;

    AutoPtr<ISimpleDateFormat> mDateFormat;
    String mLastText;
    String mDatePattern;
    AutoPtr<IBroadcastReceiver> mIntentReceiver;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CDATEVIEW_H__
