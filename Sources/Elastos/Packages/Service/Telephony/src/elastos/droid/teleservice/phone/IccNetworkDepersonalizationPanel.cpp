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

#include "elastos/droid/teleservice/phone/IccNetworkDepersonalizationPanel.h"
#include "elastos/droid/teleservice/phone/SpecialCharSequenceMgr.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IIccCard;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::Text::EIID_INoCopySpan;
using Elastos::Droid::Text::Method::IKeyListener;
using Elastos::Droid::Text::Method::IDialerKeyListener;
using Elastos::Droid::Text::Method::IDialerKeyListenerHelper;
using Elastos::Droid::Text::Method::CDialerKeyListenerHelper;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_INTERFACE_IMPL_2(IccNetworkDepersonalizationPanel::MyTextWatcher, Object, ITextWatcher, INoCopySpan)

ECode IccNetworkDepersonalizationPanel::MyTextWatcher::AfterTextChanged(
    /* [in] */ IEditable* buffer)
{
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);

    String str;
    ICharSequence::Probe(buffer)->ToString(&str);
    if (SpecialCharSequenceMgr::HandleChars(context, str)) {
        AutoPtr<ICharSequence> text;
        ITextView::Probe(mHost->mPinEntry)->GetText((ICharSequence**)&text);
        IEditable::Probe(text)->Clear();
    }
    return NOERROR;
}

ECode IccNetworkDepersonalizationPanel::MyRunnable::Run()
{
    mHost->HideAlert();

    AutoPtr<ICharSequence> text;
    ITextView::Probe(mHost->mPinEntry)->GetText((ICharSequence**)&text);
    IEditable::Probe(text)->Clear();
    Boolean res;
    return IView::Probe(mHost->mPinEntry)->RequestFocus(&res);
}

ECode IccNetworkDepersonalizationPanel::MyRunnable2::Run()
{
    return mHost->Dismiss();
}

IccNetworkDepersonalizationPanel::MyHandler::MyHandler(
    /* [in] */ IccNetworkDepersonalizationPanel* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode IccNetworkDepersonalizationPanel::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    if (what == EVENT_ICC_NTWRK_DEPERSONALIZATION_RESULT) {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        AutoPtr<AsyncResult> res = (AsyncResult*)IAsyncResult::Probe(obj);
        if (res->mException != NULL) {
            if (DBG) mHost->Log(String("network depersonalization request failure."));
            mHost->IndicateError();
            AutoPtr<IRunnable> r = new MyRunnable(mHost);
            Boolean tmp;
            PostDelayed(r, 3000, &tmp);
        }
        else {
            if (DBG) mHost->Log(String("network depersonalization success."));
            mHost->IndicateSuccess();
            AutoPtr<IRunnable> r = new MyRunnable2(mHost);
            Boolean tmp;
            PostDelayed(r, 3000, &tmp);
        }
    }
    return NOERROR;
}

ECode IccNetworkDepersonalizationPanel::MyViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<ICharSequence> text;
    ITextView::Probe(mHost->mPinEntry)->GetText((ICharSequence**)&text);
    String pin;
    text->ToString(&pin);

    if (TextUtils::IsEmpty(pin)) {
        return NOERROR;
    }

    if (DBG) {
        StringBuilder sb;
        sb += "requesting network depersonalization with code ";
        sb += pin;
        mHost->Log(sb.ToString());
    }

    AutoPtr<IIccCard> card;
    mHost->mPhone->GetIccCard((IIccCard**)&card);
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> m;
    helper->Obtain(mHost->mHandler, EVENT_ICC_NTWRK_DEPERSONALIZATION_RESULT, (IMessage**)&m);
    assert(0);
    //card->SupplyNetworkDepersonalization(pin, m);
    mHost->IndicateBusy();
    return NOERROR;
}

ECode IccNetworkDepersonalizationPanel::MyViewOnClickListener2::OnClick(
    /* [in] */ IView* v)
{
    if (DBG) mHost->Log(String("mDismissListener: skipping depersonalization..."));
    return mHost->Dismiss();
}

const Boolean IccNetworkDepersonalizationPanel::DBG = FALSE;

const Int32 IccNetworkDepersonalizationPanel::EVENT_ICC_NTWRK_DEPERSONALIZATION_RESULT = 100;

IccNetworkDepersonalizationPanel::IccNetworkDepersonalizationPanel(
    /* [in] */ IContext* context)
{
    IccPanel::constructor(context);

    mPinEntryWatcher = new MyTextWatcher(this);

    //handler for unlock function results
    mHandler = new MyHandler(this);

    mUnlockListener = new MyViewOnClickListener(this);

    mDismissListener = new MyViewOnClickListener2(this);
}

ECode IccNetworkDepersonalizationPanel::OnCreate(
    /* [in] */ IBundle* icicle)
{
    IccPanel::OnCreate(icicle);
    SetContentView(R::layout::sim_ndp);

    // PIN entry text field
    AutoPtr<IView> view;
    FindViewById(R::id::pin_entry, (IView**)&view);
    mPinEntry = IEditText::Probe(view);

    AutoPtr<IDialerKeyListenerHelper> helper;
    CDialerKeyListenerHelper::AcquireSingleton((IDialerKeyListenerHelper**)&helper);
    AutoPtr<IDialerKeyListener> listener;
    helper->GetInstance((IDialerKeyListener**)&listener);
    ITextView::Probe(mPinEntry)->SetKeyListener(IKeyListener::Probe(listener));
    IView::Probe(mPinEntry)->SetOnClickListener(mUnlockListener);

    // Attach the textwatcher
    AutoPtr<ICharSequence> text;
    ITextView::Probe(mPinEntry)->GetText((ICharSequence**)&text);
    AutoPtr<ISpannable> span = ISpannable::Probe(text);
    Int32 length;
    text->GetLength(&length);
    span->SetSpan(mPinEntryWatcher, 0, length, ISpanned::SPAN_INCLUSIVE_INCLUSIVE);

    AutoPtr<IView> view2;
    FindViewById(R::id::entry_panel, (IView**)&view2);
    mEntryPanel = ILinearLayout::Probe(view2);

    AutoPtr<IView> view3;
    FindViewById(R::id::ndp_unlock, (IView**)&view3);
    mUnlockButton = IButton::Probe(view3);
    IView::Probe(mUnlockButton)->SetOnClickListener(mUnlockListener);

    // The "Dismiss" button is present in some (but not all) products,
    // based on the "sim_network_unlock_allow_dismiss" resource.
    AutoPtr<IView> view4;
    FindViewById(R::id::ndp_dismiss, (IView**)&view4);
    mDismissButton = IButton::Probe(view4);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Boolean res;
    assert(0);
    //resources->GetBoolean(R::bool_::sim_network_unlock_allow_dismiss, &res);
    if (res) {
        if (DBG) Log(String("Enabling 'Dismiss' button..."));
        IView::Probe(mDismissButton)->SetVisibility(IView::VISIBLE);
        IView::Probe(mDismissButton)->SetOnClickListener(mDismissListener);
    }
    else {
        if (DBG) Log(String("Removing 'Dismiss' button..."));
        IView::Probe(mDismissButton)->SetVisibility(IView::GONE);
    }

    //status panel is used since we're having problems with the alert dialog.
    AutoPtr<IView> view5;
    FindViewById(R::id::status_panel, (IView**)&view5);
    mStatusPanel = ILinearLayout::Probe(view5);
    AutoPtr<IView> view6;
    FindViewById(R::id::status_text, (IView**)&view6);
    mStatusText = ITextView::Probe(view6);

    mPhone = PhoneGlobals::GetPhone();
    return NOERROR;
}

ECode IccNetworkDepersonalizationPanel::OnStart()
{
    return IccPanel::OnStart();
}

ECode IccNetworkDepersonalizationPanel::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (keyCode == IKeyEvent::KEYCODE_BACK) {
        *result = TRUE;
        return NOERROR;
    }

    return IccPanel::OnKeyDown(keyCode, event, result);
}

void IccNetworkDepersonalizationPanel::IndicateBusy()
{
    mStatusText->SetText(R::string::requesting_unlock);
    IView::Probe(mEntryPanel)->SetVisibility(IView::GONE);
    IView::Probe(mStatusPanel)->SetVisibility(IView::VISIBLE);
}
void IccNetworkDepersonalizationPanel::IndicateError()
{
    mStatusText->SetText(R::string::unlock_failed);
    IView::Probe(mEntryPanel)->SetVisibility(IView::GONE);
    IView::Probe(mStatusPanel)->SetVisibility(IView::VISIBLE);
}

void IccNetworkDepersonalizationPanel::IndicateSuccess()
{
    mStatusText->SetText(R::string::unlock_success);
    IView::Probe(mEntryPanel)->SetVisibility(IView::GONE);
    IView::Probe(mStatusPanel)->SetVisibility(IView::VISIBLE);
}

void IccNetworkDepersonalizationPanel::HideAlert()
{
    IView::Probe(mEntryPanel)->SetVisibility(IView::VISIBLE);
    IView::Probe(mStatusPanel)->SetVisibility(IView::GONE);
}

void IccNetworkDepersonalizationPanel::Log(
    /* [in] */ const String& msg)
{
    StringBuilder sb;
    sb += "[IccNetworkDepersonalizationPanel] ";
    sb += msg;
    Logger::V(TAG, sb.ToString());
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos