
#include "Elastos.Droid.Net.h"
#include "elastos/droid/settings/notification/CZenModeAutomaticConditionSelection.h"
#include "elastos/droid/settings/notification/CZenModeAutomaticConditionSelectionConditionListener.h"
#include "elastos/droid/os/ServiceManager.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Service::Notification::EIID_ICondition;
using Elastos::Droid::Animation::CLayoutTransition;
using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Service::Notification::IConditionHelper;
using Elastos::Droid::Service::Notification::CConditionHelper;
using Elastos::Droid::Service::Notification::EIID_IIConditionListener;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::CCheckBox;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

//===============================================================================
//                  CZenModeAutomaticConditionSelection::ConditionListener
//===============================================================================

CAR_INTERFACE_IMPL_2(CZenModeAutomaticConditionSelection::ConditionListener, Object, IIConditionListener, IBinder)

CZenModeAutomaticConditionSelection::ConditionListener::ConditionListener()
{}

ECode CZenModeAutomaticConditionSelection::ConditionListener::constructor(
    /* [in] */ ILinearLayout* host)
{
    mHost = (CZenModeAutomaticConditionSelection*)host;
    return NOERROR;
}

CZenModeAutomaticConditionSelection::ConditionListener::~ConditionListener()
{}

ECode CZenModeAutomaticConditionSelection::ConditionListener::OnConditionsReceived(
    /* [in] */ ArrayOf<ICondition*>* conditions)
{
    if (conditions == NULL || conditions->GetLength() == 0) return NOERROR;
    AutoPtr<IArrayOf> arrayOf = CoreUtils::Convert(conditions, EIID_ICondition);
    AutoPtr<IMessage> message;
    mHost->mHandler->ObtainMessage(H::CONDITIONS, arrayOf, (IMessage**)&message);
    return message->SendToTarget();
}

//===============================================================================
//                  CZenModeAutomaticConditionSelection::H
//===============================================================================

const Int32 CZenModeAutomaticConditionSelection::H::CONDITIONS = 1;

CZenModeAutomaticConditionSelection::H::H()
{}

ECode CZenModeAutomaticConditionSelection::H::constructor(
    /* [in] */ CZenModeAutomaticConditionSelection* host)
{
    mHost = host;
    return Handler::constructor();
}

ECode CZenModeAutomaticConditionSelection::H::HandleMessage(
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
//                  CZenModeAutomaticConditionSelection::MyOnCheckedChangeListener
//===============================================================================

CAR_INTERFACE_IMPL(CZenModeAutomaticConditionSelection::MyOnCheckedChangeListener, Object, ICompoundButtonOnCheckedChangeListener)

CZenModeAutomaticConditionSelection::MyOnCheckedChangeListener::MyOnCheckedChangeListener(
    /* [in] */ CZenModeAutomaticConditionSelection* host)
    : mHost(host)
{}

ECode CZenModeAutomaticConditionSelection::MyOnCheckedChangeListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    AutoPtr<IInterface> obj;
    IView::Probe(buttonView)->GetTag((IInterface**)&obj);
    mHost->SetSelectedCondition(IUri::Probe(obj), isChecked);
    return NOERROR;
}

//===============================================================================
//                  CZenModeAutomaticConditionSelection
//===============================================================================

const String CZenModeAutomaticConditionSelection::TAG("CZenModeAutomaticConditionSelection");
const Boolean CZenModeAutomaticConditionSelection::DEBUG = TRUE;

CAR_OBJECT_IMPL(CZenModeAutomaticConditionSelection)

CZenModeAutomaticConditionSelection::CZenModeAutomaticConditionSelection()
{}

ECode CZenModeAutomaticConditionSelection::constructor(
    /* [in] */ IContext* context)
{
    mHandler = new H();
    mHandler->constructor(this);
    CArraySet::New((IArraySet**)&mSelectedConditions);
    CZenModeAutomaticConditionSelectionConditionListener::New(this, (IIConditionListener**)&mListener);

    LinearLayout::constructor(context);
    mContext = context;
    SetOrientation(VERTICAL);
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
    RefreshSelectedConditions();
    return NOERROR;
}

void CZenModeAutomaticConditionSelection::RefreshSelectedConditions()
{
    AutoPtr< ArrayOf<ICondition*> > automatic;
    ECode ec = mNoMan->GetAutomaticZenModeConditions((ArrayOf<ICondition*>**)&automatic);
    if (SUCCEEDED(ec)) {
        mSelectedConditions->Clear();
        if (automatic != NULL) {
            for (Int32 i = 0; i < automatic->GetLength(); i++) {
                AutoPtr<ICondition> c = (*automatic)[i];
                AutoPtr<IUri> id;
                c->GetId((IUri**)&id);
                mSelectedConditions->Add(id);
            }
        }
    }
    else {
        Logger::W(TAG, "Error calling getAutomaticZenModeConditions 0x%08x", ec);
    }
}

AutoPtr<ICheckBox> CZenModeAutomaticConditionSelection::NewCheckBox(
    /* [in] */ IInterface* tag)
{
    AutoPtr<ICheckBox> button;
    CCheckBox::New(mContext, (ICheckBox**)&button);
    IView::Probe(button)->SetTag(tag);
    AutoPtr<MyOnCheckedChangeListener> listener = new MyOnCheckedChangeListener(this);
    ICompoundButton::Probe(button)->SetOnCheckedChangeListener(listener);
    AddView(IView::Probe(button));
    return button;
}

void CZenModeAutomaticConditionSelection::SetSelectedCondition(
    /* [in] */ IUri* conditionId,
    /* [in] */ Boolean selected)
{
    if (DEBUG) Logger::D(TAG, "setSelectedCondition conditionId= %s selected=%d", TO_CSTR(conditionId), selected);
    if (selected) {
        mSelectedConditions->Add(conditionId);
    }
    else {
        mSelectedConditions->Remove(conditionId);
    }

    Int32 size;
    mSelectedConditions->GetSize(&size);
    AutoPtr< ArrayOf<IUri*> > automatic = ArrayOf<IUri*>::Alloc(size);
    for (Int32 i = 0; i < automatic->GetLength(); i++) {
        AutoPtr<IInterface> obj;
        mSelectedConditions->GetValueAt(i, (IInterface**)&obj);
        automatic->Set(i, IUri::Probe(obj));
    }
    ECode ec = mNoMan->SetAutomaticZenModeConditions(automatic);
    if (FAILED(ec)) {
        Logger::W(TAG, "Error calling setAutomaticZenModeConditions 0x%08x", ec);
    }
}

ECode CZenModeAutomaticConditionSelection::OnAttachedToWindow()
{
    LinearLayout::OnAttachedToWindow();
    RequestZenModeConditions(ICondition::FLAG_RELEVANT_ALWAYS);
    return NOERROR;
}

ECode CZenModeAutomaticConditionSelection::OnDetachedFromWindow()
{
    LinearLayout::OnDetachedFromWindow();
    RequestZenModeConditions(0 /*none*/);
    return NOERROR;
}

void CZenModeAutomaticConditionSelection::RequestZenModeConditions(
    /* [in] */ Int32 relevance)
{
    AutoPtr<IConditionHelper> helper;
    CConditionHelper::AcquireSingleton((IConditionHelper**)&helper);
    String str;
    helper->RelevanceToString(relevance, &str);
    if (DEBUG) Logger::D(TAG, "requestZenModeConditions %s", str.string());
    ECode ec = mNoMan->RequestZenModeConditions(mListener, relevance);
    if (FAILED(ec)) {
        Logger::W(TAG, "Error calling requestZenModeConditions 0x%08x", ec);
    }
}

void CZenModeAutomaticConditionSelection::HandleConditions(
    /* [in] */ ArrayOf<ICondition*>* conditions)
{
    for (Int32 i = 0; i < conditions->GetLength(); i++) {
        AutoPtr<ICondition> c = (*conditions)[i];
        AutoPtr<IUri> id;
        c->GetId((IUri**)&id);
        AutoPtr<IView> view;
        FindViewWithTag(id, (IView**)&view);
        ICheckBox* v = ICheckBox::Probe(view);
        Int32 state;
        c->GetState(&state);
        if (state != ICondition::STATE_ERROR) {
            if (v == NULL) {
                v = NewCheckBox(id);
            }
        }
        if (v != NULL) {
            String summary;
            c->GetSummary(&summary);
            ITextView::Probe(v)->SetText(CoreUtils::Convert(summary));
            IView::Probe(v)->SetEnabled(state != ICondition::STATE_ERROR);
            Boolean res;
            mSelectedConditions->Contains(id, &res);
            ICheckable::Probe(v)->SetChecked(res);
        }
    }
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos