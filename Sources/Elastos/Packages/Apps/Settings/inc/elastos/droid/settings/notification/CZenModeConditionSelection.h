
#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODECONDITIONSELECTION_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODECONDITIONSELECTION_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Service.h"
#include "_Elastos_Droid_Settings_Notification_CZenModeConditionSelection.h"
#include "elastos/droid/widget/RadioGroup.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Service::Notification::ICondition;
using Elastos::Droid::Service::Notification::IIConditionListener;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::RadioGroup;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CarClass(CZenModeConditionSelection)
    , public RadioGroup
{
public:
    class ConditionListener
        : public Object
        , public IIConditionListener
        , public IBinder
    {
    public:
        TO_STRING_IMPL("CZenModeConditionSelection::ConditionListener");

        CAR_INTERFACE_DECL()

        ConditionListener();

        CARAPI constructor(
            /* [in] */ ILinearLayout* host);

        ~ConditionListener();

        //@Override
        CARAPI OnConditionsReceived(
            /* [in] */ ArrayOf<ICondition*>* conditions);

    private:
        CZenModeConditionSelection* mHost;
    };

private:
    class H
        : public Handler
    {
        friend class ConditionListener;
    public:
        TO_STRING_IMPL("CZenModeConditionSelection::H")

        H();

        CARAPI constructor(
            /* [in] */ CZenModeConditionSelection* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        static const Int32 CONDITIONS;// = 1;

        CZenModeConditionSelection* mHost;
    };

    class MyOnCheckedChangeListener
        : public Object
        , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        TO_STRING_IMPL("CZenModeConditionSelection::MyOnCheckedChangeListener")

        CAR_INTERFACE_DECL()

        MyOnCheckedChangeListener(
            /* [in] */ CZenModeConditionSelection* host,
            /* [in] */ IRadioButton* button);

        //@Override
        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);

    private:
        CZenModeConditionSelection* mHost;
        AutoPtr<IRadioButton> mButton;
    };

public:
    CAR_OBJECT_DECL()

    CZenModeConditionSelection();

    CARAPI constructor(
        /* [in] */ IContext* context);

    virtual CARAPI ConfirmCondition();

protected:
    //@Override
    CARAPI OnAttachedToWindow();

    //@Override
    CARAPI OnDetachedFromWindow();

    virtual CARAPI_(void) RequestZenModeConditions(
        /* [in] */ Int32 relevance);

    virtual CARAPI_(void) HandleConditions(
        /* [in] */ ArrayOf<ICondition*>* conditions);

    virtual CARAPI_(void) HandleCondition(
        /* [in] */ ICondition* c);

    virtual CARAPI_(void) SetCondition(
        /* [in] */ ICondition* c);

private:
    CARAPI_(AutoPtr<IRadioButton>) NewRadioButton(
        /* [in] */ ICondition* condition);

private:
    static const String TAG;// = "CZenModeConditionSelection";
    static const Boolean DEBUG;// = TRUE;

    AutoPtr<IINotificationManager> mNoMan;
    AutoPtr<H> mHandler;// = new H();
    AutoPtr<IContext> mContext;
    // List<Condition> mConditions;
    AutoPtr<IList> mConditions;
    AutoPtr<ICondition> mCondition;

    AutoPtr<IIConditionListener> mListener;// = new IConditionListener->Stub()
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODECONDITIONSELECTION_H__