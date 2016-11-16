#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODEAUTOMATICCONDITIONSELECTION_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODEAUTOMATICCONDITIONSELECTION_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Service.h"
#include "_Elastos_Droid_Settings_Notification_CZenModeAutomaticConditionSelection.h"
#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Service::Notification::ICondition;
using Elastos::Droid::Service::Notification::IIConditionListener;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CarClass(CZenModeAutomaticConditionSelection)
    , public LinearLayout
{
public:
    class ConditionListener
        : public Object
        , public IIConditionListener
        , public IBinder
    {
    public:
        TO_STRING_IMPL("CZenModeAutomaticConditionSelection::ConditionListener");

        CAR_INTERFACE_DECL()

        ConditionListener();

        CARAPI constructor(
            /* [in] */ ILinearLayout* host);

        ~ConditionListener();

        //@Override
        CARAPI OnConditionsReceived(
            /* [in] */ ArrayOf<ICondition*>* conditions);

    private:
        CZenModeAutomaticConditionSelection* mHost;
    };

private:
    class H
        : public Handler
    {
        friend class ConditionListener;
    public:
        TO_STRING_IMPL("CZenModeAutomaticConditionSelection::H")

        H();

        CARAPI constructor(
            /* [in] */ CZenModeAutomaticConditionSelection* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        static const Int32 CONDITIONS;

        CZenModeAutomaticConditionSelection* mHost;
    };

    class MyOnCheckedChangeListener
        : public Object
        , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        TO_STRING_IMPL("CZenModeAutomaticConditionSelection::MyOnCheckedChangeListener")

        CAR_INTERFACE_DECL()

        MyOnCheckedChangeListener(
            /* [in] */ CZenModeAutomaticConditionSelection* host);

        //@Override
        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);

    private:
        CZenModeAutomaticConditionSelection* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CZenModeAutomaticConditionSelection();

    CARAPI constructor(
        /* [in] */ IContext* context);

protected:
    //@Override
    CARAPI OnAttachedToWindow();

    //@Override
    CARAPI OnDetachedFromWindow();

    virtual CARAPI_(void) RequestZenModeConditions(
        /* [in] */ Int32 relevance);

    virtual CARAPI_(void) HandleConditions(
        /* [in] */ ArrayOf<ICondition*>* conditions);

private:
    CARAPI_(void) RefreshSelectedConditions();

    CARAPI_(AutoPtr<ICheckBox>) NewCheckBox(
        /* [in] */ IInterface* tag);

    CARAPI_(void) SetSelectedCondition(
        /* [in] */ IUri* conditionId,
        /* [in] */ Boolean selected);

private:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<IINotificationManager> mNoMan;
    AutoPtr<H> mHandler;
    AutoPtr<IContext> mContext;
    // ArraySet<Uri> mSelectedConditions = new ArraySet<Uri>();
    AutoPtr<IArraySet> mSelectedConditions;

    AutoPtr<IIConditionListener> mListener;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODEAUTOMATICCONDITIONSELECTION_H__