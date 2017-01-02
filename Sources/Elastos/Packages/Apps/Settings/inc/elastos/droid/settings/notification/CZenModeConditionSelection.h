//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
        static const Int32 CONDITIONS;

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
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<IINotificationManager> mNoMan;
    AutoPtr<H> mHandler;
    AutoPtr<IContext> mContext;
    // List<Condition> mConditions;
    AutoPtr<IList> mConditions;
    AutoPtr<ICondition> mCondition;

    AutoPtr<IIConditionListener> mListener;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODECONDITIONSELECTION_H__