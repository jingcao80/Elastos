
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_COMPATMODEBUTTON_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_COMPATMODEBUTTON_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/widget/TextView.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::TextView;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class DateView : public TextView
{
private:
    class DateViewBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
         DateViewBroadcastReceiver(
            /* [in] */ DateView* bar);

    protected:
        virtual CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("DateViewBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        DateView* mHost;
    };

public:
    DateView();

    DateView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

    virtual void OnWindowVisibilityChanged(
        /* [in] */ Int32 visibility);

    virtual CARAPI OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

    virtual CARAPI_(Int32) GetSuggestedMinimumWidth();

    void UpdateClock();

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

private:
    Boolean IsVisible();

    void SetUpdates();

private:
    static const String TAG;// = "DateView";

    static const Int32 DATE_TEXTVIEW_SCALER;

    Boolean mAttachedToWindow;
    Boolean mWindowVisible;
    Boolean mUpdating;

    AutoPtr<IBroadcastReceiver> mIntentReceiver;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_COMPATMODEBUTTON_H__
