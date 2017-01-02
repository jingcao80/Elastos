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

#include "Elastos.Droid.Net.h"
#include "elastos/droid/settings/notification/CZenModeConditionSelection.h"
#include "elastos/droid/settings/notification/CZenModeConditionSelectionConditionListener.h"
#include "elastos/droid/os/ServiceManager.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::Animation::CLayoutTransition;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Service::Notification::EIID_ICondition;
using Elastos::Droid::Service::Notification::IConditionHelper;
using Elastos::Droid::Service::Notification::CConditionHelper;
using Elastos::Droid::Service::Notification::IZenModeConfigHelper;
using Elastos::Droid::Service::Notification::CZenModeConfigHelper;
using Elastos::Droid::Service::Notification::IZenModeConfig;
using Elastos::Droid::Service::Notification::EIID_IIConditionListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::CRadioButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

//===============================================================================
//                  CZenModeConditionSelection::ConditionListener
//===============================================================================

CAR_INTERFACE_IMPL_2(CZenModeConditionSelection::ConditionListener, Object, IIConditionListener, IBinder)

CZenModeConditionSelection::ConditionListener::ConditionListener()
{}

ECode CZenModeConditionSelection::ConditionListener::constructor(
    /* [in] */ ILinearLayout* host)
{
    mHost = (CZenModeConditionSelection*)host;
    return NOERROR;
}

CZenModeConditionSelection::ConditionListener::~ConditionListener()
{}

ECode CZenModeConditionSelection::ConditionListener::OnConditionsReceived(
    /* [in] */ ArrayOf<ICondition*>* conditions)
{
    if (conditions == NULL || conditions->GetLength() == 0) return NOERROR;
    AutoPtr<IArrayOf> arrayOf = CoreUtils::Convert(conditions, EIID_ICondition);
    AutoPtr<IMessage> message;
    mHost->mHandler->ObtainMessage(H::CONDITIONS, arrayOf, (IMessage**)&message);
    return message->SendToTarget();
}

//===============================================================================
//                  CZenModeConditionSelection::H
//===============================================================================

const Int32 CZenModeConditionSelection::H::CONDITIONS = 1;

CZenModeConditionSelection::H::H()
{}

ECode CZenModeConditionSelection::H::constructor(
    /* [in] */ CZenModeConditionSelection* host)
{
    mHost = host;
    return Handler::constructor();
}

ECode CZenModeConditionSelection::H::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    if (what == CONDITIONS) {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        IArrayOf* objArrayOf = IArrayOf::Probe(obj);
        AutoPtr< ArrayOf<ICondition*> > array = CoreUtils::Convert<ICondition>(objArrayOf);
        mHost->HandleConditions(array);
    }
    return NOERROR;
}

//===============================================================================
//                  CZenModeConditionSelection::MyOnCheckedChangeListener
//===============================================================================

CAR_INTERFACE_IMPL(CZenModeConditionSelection::MyOnCheckedChangeListener, Object, ICompoundButtonOnCheckedChangeListener)

CZenModeConditionSelection::MyOnCheckedChangeListener::MyOnCheckedChangeListener(
    /* [in] */ CZenModeConditionSelection* host,
    /* [in] */ IRadioButton* button)
    : mHost(host)
    , mButton(button)
{}

ECode CZenModeConditionSelection::MyOnCheckedChangeListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    if (isChecked) {
        AutoPtr<IInterface> obj;
        IView::Probe(mButton)->GetTag((IInterface**)&obj);
        mHost->SetCondition(ICondition::Probe(obj));
    }
    return NOERROR;
}

//===============================================================================
//                  CZenModeConditionSelection
//===============================================================================

const String CZenModeConditionSelection::TAG("CZenModeConditionSelection");
const Boolean CZenModeConditionSelection::DEBUG = TRUE;

CAR_OBJECT_IMPL(CZenModeConditionSelection)

CZenModeConditionSelection::CZenModeConditionSelection()
{}

ECode CZenModeConditionSelection::constructor(
    /* [in] */ IContext* context)
{
    mHandler = new H();
    mHandler->constructor(this);
    CZenModeConditionSelectionConditionListener::New(this, (IIConditionListener**)&mListener);

    RadioGroup::constructor(context);
    mContext = context;
    CArrayList::New((IList**)&mConditions);
    AutoPtr<ILayoutTransition> lt;
    CLayoutTransition::New((ILayoutTransition**)&lt);
    SetLayoutTransition(lt);
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    Int32 p;
    resources->GetDimensionPixelSize(R::dimen::content_margin_left, &p);
    SetPadding(p, p, p, 0);
    mNoMan = IINotificationManager::Probe(
            ServiceManager::GetService(IContext::NOTIFICATION_SERVICE));
    AutoPtr<IRadioButton> b = NewRadioButton(NULL);
    String str;
    mContext->GetString(Elastos::Droid::R::string::zen_mode_forever, &str);
    ITextView::Probe(b)->SetText(CoreUtils::Convert(str));
    ICheckable::Probe(b)->SetChecked(TRUE);
    AutoPtr< ArrayOf<Int32> > MINUTE_BUCKETS;
    AutoPtr<IZenModeConfigHelper> helper;
    CZenModeConfigHelper::AcquireSingleton((IZenModeConfigHelper**)&helper);
    helper->GetMINUTE_BUCKETS((ArrayOf<Int32>**)&MINUTE_BUCKETS);
    for (Int32 i = MINUTE_BUCKETS->GetLength() - 1; i >= 0; --i) {
        AutoPtr<ICondition> condition;
        helper->ToTimeCondition((*MINUTE_BUCKETS)[i], (ICondition**)&condition);
        HandleCondition(condition);
    }
    return NOERROR;
}

AutoPtr<IRadioButton> CZenModeConditionSelection::NewRadioButton(
    /* [in] */ ICondition* condition)
{
    AutoPtr<IRadioButton> button;
    CRadioButton::New(mContext, (IRadioButton**)&button);
    IView::Probe(button)->SetTag(condition);
    AutoPtr<MyOnCheckedChangeListener> listener = new MyOnCheckedChangeListener(this, button);
    ICompoundButton::Probe(button)->SetOnCheckedChangeListener(listener);
    ViewGroup::AddView(IView::Probe(button));
    return button;
}

ECode CZenModeConditionSelection::OnAttachedToWindow()
{
    RadioGroup::OnAttachedToWindow();
    RequestZenModeConditions(ICondition::FLAG_RELEVANT_NOW);
    return NOERROR;
}

ECode CZenModeConditionSelection::OnDetachedFromWindow()
{
    RadioGroup::OnDetachedFromWindow();
    RequestZenModeConditions(0 /*none*/);
    return NOERROR;
}

void CZenModeConditionSelection::RequestZenModeConditions(
    /* [in] */ Int32 relevance)
{
    if (DEBUG) {
        AutoPtr<IConditionHelper> helper;
        CConditionHelper::AcquireSingleton((IConditionHelper**)&helper);
        String str;
        helper->RelevanceToString(relevance, &str);
        Logger::D(TAG, "requestZenModeConditions %s", str.string());
    }

    ECode ec = mNoMan->RequestZenModeConditions(mListener, relevance);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        // noop
    }
}

void CZenModeConditionSelection::HandleConditions(
    /* [in] */ ArrayOf<ICondition*>* conditions)
{
    for (Int32 i = 0; i < conditions->GetLength(); ++i) {
        AutoPtr<ICondition> c = (*conditions)[i];
        HandleCondition(c);
    }
}

void CZenModeConditionSelection::HandleCondition(
    /* [in] */ ICondition* c)
{
    Boolean res;
    mConditions->Contains(c, &res);
    if (res) return;

    AutoPtr<IUri> id;
    c->GetId((IUri**)&id);
    AutoPtr<IView> tmp;
    FindViewWithTag(id, (IView**)&tmp);
    IRadioButton* v = IRadioButton::Probe(tmp);
    Int32 state;
    c->GetState(&state);
    if (state == ICondition::STATE_TRUE || state == ICondition::STATE_UNKNOWN) {
        if (v == NULL) {
            v = NewRadioButton(c);
        }
    }
    if (v != NULL) {
        String summary;
        c->GetSummary(&summary);
        ITextView::Probe(v)->SetText(CoreUtils::Convert(summary));
        IView::Probe(v)->SetEnabled(state == ICondition::STATE_TRUE);
    }
    mConditions->Add(c);
}

void CZenModeConditionSelection::SetCondition(
    /* [in] */ ICondition* c)
{
    if (DEBUG) Logger::D(TAG, "setCondition %s", TO_CSTR(c));
    mCondition = c;
}

ECode CZenModeConditionSelection::ConfirmCondition()
{
    if (DEBUG) Logger::D(TAG, "confirmCondition %s", TO_CSTR(mCondition));

    ECode ec = mNoMan->SetZenModeCondition(mCondition);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        // noop
    }
    return NOERROR;
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos