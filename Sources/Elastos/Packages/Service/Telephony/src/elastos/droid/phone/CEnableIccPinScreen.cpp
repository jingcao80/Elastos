
#include "elastos/droid/phone/CEnableIccPinScreen.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CEnableIccPinScreen::MyHandler::MyHandler(
    /* [in] */ CEnableIccPinScreen* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CEnableIccPinScreen::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case ENABLE_ICC_PIN_COMPLETE:
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IAsyncResult> ar = IAsyncResult::Probe(obj);
            mHost->HandleResult(ar);
            break;
    }

    return NOERROR;

}

CAR_INTERFACE_IMPL(CEnableIccPinScreen::MyViewOnClickListener, Object, IViewOnClickListener)

ECode CEnableIccPinScreen::MyViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<ICharSequence> text;
    mHost->mPinField->GetText((ICharSequence**)&text);
    if (TextUtils::IsEmpty(text)) {
        return NOERROR;
    }

    AutoPtr<IResources> resources;
    mHost->GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> text2;
    resources->GetText(R.string.enable_in_progress, (ICharSequence**)&text2);
    mHost->ShowStatus(text2);

    mHost->EnableIccPin();
    return NOERROR;
}

ECode CEnableIccPinScreen::MyRunnable::Run()
{
    return mHost->Finish();
}

const String CEnableIccPinScreen::TAG("CEnableIccPinScreen");// = PhoneGlobals.TAG;

const Int32 CEnableIccPinScreen::ENABLE_ICC_PIN_COMPLETE = 100;
const Boolean CEnableIccPinScreen::DBG = FALSE;

CAR_INTERFACE_IMPL(CEnableIccPinScreen, Activity, IEnableIccPinScreen)

CAR_OBJECT_IMPL(CEnableIccPinScreen)

CEnableIccPinScreen::CEnableIccPinScreen()
    : mEnable(FALSE)
{
    mHandler = new MyHandler(this);

    mClicked = new MyViewOnClickListener(this);
}

ECode CEnableIccPinScreen::constructor()
{
    Activity::constructor();
}

ECode CEnableIccPinScreen::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);

    SetContentView(R.layout.enable_sim_pin_screen);
    SetupView();

    PhoneGlobals::GetPhone((IPhone**)&mPhone);
    AutoPtr<IIccCard> card;
    mPhone->GetIccCard((IIccCard**)&card);
    card->GetIccLockEnabled(&mEnable);
    mEnable = !mEnable;

    Int32 id = mEnable ? R.string.enable_sim_pin : R.string.disable_sim_pin;
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> cchar;
    resources->GetText(id, (ICharSequence**)&cchar);
    return SetTitle(cchar);
}

void CEnableIccPinScreen::SetupView()
{
    AutoPtr<IView> view;
    FindViewById(R.id.pin, (IView**)&view);
    mPinField = IEditText::Probe(view);

    AutoPtr<IDigitsKeyListenerHelper> helper;
    CDigitsKeyListenerHelper::New((IDigitsKeyListenerHelper**)&helper);
    AutoPtr<IDigitsKeyListener> ret;
    helper->GetInstance((IDigitsKeyListener**)&ret);
    mPinField->SetKeyListener(ret);
    mPinField->SetMovementMethod(NULL);
    mPinField->SetOnClickListener(mClicked);

    AutoPtr<IView> view2;
    FindViewById(R.id.pinc, (IView**)&view2);
    mPinFieldContainer = ILinearLayout::Probe(view2);

    AutoPtr<IView> view3;
    FindViewById(R.id.status, (IView**)&view3);
    mStatusField = ITextView::Probe(view3);
}

void CEnableIccPinScreen::ShowStatus(
    /* [in] */ ICharSequence* statusMsg)
{
    if (statusMsg != NULL) {
        mStatusField->SetText(statusMsg);
        mStatusField->SetVisibility(IView::VISIBLE);
        mPinFieldContainer->SetVisibility(IView::GONE);
    }
    else {
        mPinFieldContainer->SetVisibility(IView::VISIBLE);
        mStatusField->SetVisibility(IView::GONE);
    }
}

String CEnableIccPinScreen::GetPin()
{
    AutoPtr<ICharSequence> cchar;
    mPinField->GetText((ICharSequence**)&cchar);
    String str;
    cchar->ToString(&str)
    return str;
}

void CEnableIccPinScreen::EnableIccPin()
{
    AutoPtr<IMessage> callback;
    Message::Obtain(mHandler, ENABLE_ICC_PIN_COMPLETE, (IMessage**)&callback);

    if (DBG) Log(String("enableIccPin:"));

    AutoPtr<IIccCard> card;
    mPhone->GetIccCard((IIccCard**)&card);
    card->SetIccLockEnabled(mEnable, GetPin(), callback);

    if (DBG) Log(String("enableIccPin: please wait..."));
}

void CEnableIccPinScreen::HandleResult(
    /* [in] */ IAsyncResult* ar)
{
    if (ar.exception == null) {
        if (DBG) log("handleResult: success!");
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr<ICharSequence> cchar;
        resources->GetText(mEnable ? R.string.enable_pin_ok : R.string.disable_pin_ok, (ICharSequence**)&cchar);
        ShowStatus(cchar);
    }
    else if (ar.exception instanceof CommandException
            /* && ((CommandException)ar.exception).getCommandError() ==
                CommandException.Error.GENERIC_FAILURE */ ) {
        if (DBG) log("handleResult: failed!");
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr<ICharSequence> cchar;
        resources->GetText(R.string.pin_failed, (ICharSequence**)&cchar);
        ShowStatus(cchar);
    }

    AutoPtr<IRunnable> r = new MyRunnable(this);
    mHandler->PostDelayed(r, 3000);
    return NOERROR;
}

void CEnableIccPinScreen::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, String("[EnableIccPin] ") + msg);
}


} // namespace Phone
} // namespace Droid
} // namespace Elastos