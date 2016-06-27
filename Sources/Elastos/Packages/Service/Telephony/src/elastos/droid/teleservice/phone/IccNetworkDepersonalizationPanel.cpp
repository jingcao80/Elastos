
#include "elastos/droid/teleservice/phone/IccNetworkDepersonalizationPanel.h"

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

ECodeIccNetworkDepersonalizationPanel::MyTextWatcher::AfterTextChanged(
    /* [in] */ IEditable* buffer)
{
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);

    String str;
    buffer->ToString(&str);
    Boolean res;
    if (SpecialCharSequenceMgr::HandleChars(context, str, &res), res) {
        AutoPtr<ICharSequence> text;
        mHost->mPinEntry->GetText((ICharSequence**)&text);
        text->Clear();
    }
    return NOERROR;
}

ECode IccNetworkDepersonalizationPanel::MyRunnable::Run()
{
    mHost->HideAlert();

    AutoPtr<ICharSequence> text;
    mHost->mPinEntry->GetText((ICharSequence**)&text);
    text->Clear();
    return mHost->mPinEntry->RequestFocus();
}

ECode IccNetworkDepersonalizationPanel::MyRunnable2::Run()
{
    return mHost->dismiss();
}

IccNetworkDepersonalizationPanel::MyHandler::MyHandler(
    /* [in] */ IccNetworkDepersonalizationPanel* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode ECodeIccNetworkDepersonalizationPanel::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    if (what == EVENT_ICC_NTWRK_DEPERSONALIZATION_RESULT) {
        AsyncResult res = (AsyncResult) msg.obj;
        if (res.exception != null) {
            if (DBG) mHost->Log("network depersonalization request failure.");
            mHost->IndicateError();
            AutoPtr<IRunnable> r = new MyRunnable(mHost);
            PostDelayed(r, 3000);
        }
        else {
            if (DBG) mHost->Log("network depersonalization success.");
            mHost->IndicateSuccess();
            AutoPtr<IRunnable> r = new MyRunnable2(mHost);
            PostDelayed(r, 3000);
        }
    }
    return NOERROR;
}

ECode IccNetworkDepersonalizationPanel::MyViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<ICharSequence> text;
    mHost->mPinEntry->GetText((ICharSequence**)&text);
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
    AutoPtr<IMessage> m;
    Message::Obtain(mHandler, EVENT_ICC_NTWRK_DEPERSONALIZATION_RESULT, (IMessage**)&m);
    card->SupplyNetworkDepersonalization(pin, m);
    return mHost->IndicateBusy();
}

ECode IccNetworkDepersonalizationPanel::MyViewOnClickListener2::OnClick(
    /* [in] */ IView* v)
{
    if (DBG) mHost->Log("mDismissListener: skipping depersonalization...");
    return mHost->Dismiss();
}

static const Boolean IccNetworkDepersonalizationPanel::DBG = FALSE;

static const Int32 IccNetworkDepersonalizationPanel::EVENT_ICC_NTWRK_DEPERSONALIZATION_RESULT = 100;

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
    SetContentView(R.layout.sim_ndp);

    // PIN entry text field
    AutoPtr<IView> view;
    FindViewById(R.id.pin_entry, (IView**)&view);
    mPinEntry = IEditText::Probe(view);

    AutoPtr<IDialerKeyListenerHelper> helper;
    CDialerKeyListenerHelper::AcquireSingleton((IDialerKeyListenerHelper**)&helper);
    AutoPtr<IDialerKeyListener> listener;
    helper->GetInstance((IDialerKeyListener**)&listener);
    mPinEntry->SetKeyListener(listener);
    mPinEntry->SetOnClickListener(mUnlockListener);

    // Attach the textwatcher
    AutoPtr<ICharSequence> text;
    mPinEntry->GetText((ICharSequence**)&text);
    AutoPtr<ISpannable> span = ISpannable::Probe(text);
    span->SetSpan(mPinEntryWatcher, 0, text->GetLength(), ISpannable::SPAN_INCLUSIVE_INCLUSIVE);

    AutoPtr<IView> view2;
    FindViewById(R.id.entry_panel, (IView**)&view2);
    mEntryPanel = ILinearLayout::Probe(view2);

    AutoPtr<IView> view3;
    FindViewById(R.id.ndp_unlock, (IView**)&view3);
    mUnlockButton = IButton::Probe(view3);
    mUnlockButton->SetOnClickListener(mUnlockListener);

    // The "Dismiss" button is present in some (but not all) products,
    // based on the "sim_network_unlock_allow_dismiss" resource.
    AutoPtr<IView> view4;
    FindViewById(R.id.ndp_dismiss, (IView**)&view4);
    mDismissButton = IButton::Probe(view4);

    AutoPtr<IContex> context;
    GetContext((IContex**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Boolean res;
    resources->GetBoolean(R.bool.sim_network_unlock_allow_dismiss, &res);
    if (res) {
        if (DBG) Log("Enabling 'Dismiss' button...");
        mDismissButton->SetVisibility(IView::VISIBLE);
        mDismissButton->SetOnClickListener(mDismissListener);
    }
    else {
        if (DBG) log("Removing 'Dismiss' button...");
        mDismissButton->SetVisibility(IView::GONE);
    }

    //status panel is used since we're having problems with the alert dialog.
    AutoPtr<IView> view5;
    FindViewById(R.id.status_panel, (IView**)&view5);
    mStatusPanel = ILinearLayout::Probe(view5);
    AutoPtr<IView> view6;
    FindViewById(R.id.status_text, (IView**)&view6);
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

void IccNetworkDepersonalizationPanel::IndicateBusy();
{
    mStatusText->SetText(R.string.requesting_unlock);
    mEntryPanel->SetVisibility(IView::GONE);
    mStatusPanel->SetVisibility(IView::VISIBLE);
}
void IccNetworkDepersonalizationPanel::IndicateError();
{
    mStatusText->SetText(R.string.unlock_failed);
    mEntryPanel->SetVisibility(IView::GONE);
    mStatusPanel->SetVisibility(IView::VISIBLE);
}

void IccNetworkDepersonalizationPanel::IndicateSuccess();
{
    mStatusText->SetText(R.string.unlock_success);
    mEntryPanel->SetVisibility(IView::GONE);
    mStatusPanel->SetVisibility(IView::VISIBLE);
}

void IccNetworkDepersonalizationPanel::HideAlert();
{
    mEntryPanel->SetVisibility(IView::VISIBLE);
    mStatusPanel->SetVisibility(IView::GONE);
}

void IccNetworkDepersonalizationPanel::Log(
    /* [in] */ const String& msg);
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