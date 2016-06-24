
#include "elastos/droid/phone/CEnableFdnScreen.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CEnableFdnScreen::MyHandler::MyHandler(
    /* [in] */ CEnableFdnScreen* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CEnableFdnScreen::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case ENABLE_FDN_COMPLETE:
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IAsyncResult> ar = IAsyncResult::Probe(obj);
            mHost->HandleResult(ar);
            break;
    }

    return NOERROR;
}

CAR_INTERFACE_IMPL(CEnableFdnScreen::MyViewOnClickListener, Object, IViewOnClickListener)

ECode CEnableFdnScreen::MyViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<ICharSequence> text;
    mHost->mPin2Field->GetText((ICharSequence**)&text);
    if (TextUtils::IsEmpty(text)) {
        return NOERROR;
    }

    AutoPtr<IResources> resources;
    mHost->GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> text2;
    resources->GetText(R.string.enable_in_progress, (ICharSequence**)&text2);
    mHost->ShowStatus(text2);

    mHost->EnableFdn();
    return NOERROR;
}

ECode CEnableFdnScreen::MyRunnable::Run()
{
    return mHost->Finish();
}

const String CEnableFdnScreen::TAG("CEnableFdnScreen");// = PhoneGlobals.TAG;
const Boolean CEnableFdnScreen::DBG = FALSE;

const Int32 CEnableFdnScreen::ENABLE_FDN_COMPLETE = 100;

CAR_INTERFACE_IMPL(CEnableFdnScreen, Activity, IEnableFdnScreen)

CAR_OBJECT_IMPL(CEnableFdnScreen)

CEnableFdnScreen::CEnableFdnScreen()
    : mEnable(FALSE)
{
    mHandler = new MyHandler(this);

    mClicked = new MyViewOnClickListener(this);
}

ECode CEnableFdnScreen::constructor()
{
    return Activity::constructor();
}

ECode CEnableFdnScreen::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);

    SetContentView(R.layout.enable_fdn_screen);
    SetupView();

    PhoneGlobals::GetPhone((IPhone**)&mPhone);
    AutoPtr<IIccCard> card;
    mPhone->GetIccCard((IIccCard**)&card);
    card->GetIccFdnEnabled(&mEnable);
    mEnable = !mEnable;

    Int32 id = mEnable ? R.string.enable_fdn : R.string.disable_fdn;
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> cchar;
    resources->GetText(id, (ICharSequence**)&cchar);
    return SetTitle(cchar);
}

ECode CEnableFdnScreen::OnResume()
{
    Activity::OnResume();
    return PhoneGlobals::GetPhone((IPhone**)&mPhone);
}

void CEnableFdnScreen::SetupView()
{
    AutoPtr<IView> view;
    FindViewById(R.id.pin, (IView**)&view);
    mPin2Field = IEditText::Probe(view);

    AutoPtr<IDigitsKeyListenerHelper> helper;
    CDigitsKeyListenerHelper::New((IDigitsKeyListenerHelper**)&helper);
    AutoPtr<IDigitsKeyListener> ret;
    helper->GetInstance((IDigitsKeyListener**)&ret);
    mPin2Field->SetKeyListener(ret);
    mPin2Field->SetMovementMethod(NULL);
    mPin2Field->SetOnClickListener(mClicked);

    AutoPtr<IView> view2;
    FindViewById(R.id.pinc, (IView**)&view2);
    mPinFieldContainer = ILinearLayout::Probe(view2);

    AutoPtr<IView> view3;
    FindViewById(R.id.status, (IView**)&view3);
    mStatusField = ITextView::Probe(view3);
}

void CEnableFdnScreen::ShowStatus(
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

String CEnableFdnScreen::GetPin2()
{
    AutoPtr<ICharSequence> cchar;
    mPin2Field->GetText((ICharSequence**)&cchar);
    String str;
    cchar->ToString(&str)
    return str;
}

void CEnableFdnScreen::EnableFdn()
{
    AutoPtr<IMessage> callback;
    Message::Obtain(mHandler, ENABLE_FDN_COMPLETE, (IMessage**)&callback);

    AutoPtr<IIccCard> card;
    mPhone->GetIccCard((IIccCard**)&card);
    card->SetIccFdnEnabled(mEnable, GetPin2(), callback);

    if (DBG) Log(String("enableFdn: please wait..."));
}

void CEnableFdnScreen::HandleResult(
    /* [in] */ IAsyncResult* ar)
{
    if (ar.exception == null) {
        if (DBG) Log(String("handleResult: success!"));
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr<ICharSequence> cchar;
        resources->GetText(mEnable ? R.string.enable_fdn_ok : R.string.disable_fdn_ok, (ICharSequence**)&cchar);
        ShowStatus(cchar);
    }
    else if (ar.exception instanceof CommandException
            /* && ((CommandException)ar.exception).getCommandError() ==
                CommandException.Error.GENERIC_FAILURE */ ) {
        if (DBG) Log(String("handleResult: failed!"));
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr<ICharSequence> cchar;
        resources->GetText(R.string.pin_failed, (ICharSequence**)&cchar);
        ShowStatus(cchar);
    }

    AutoPtr<IRunnable> r;
    mHandler->PostDelayed(r, 3000);
    return NOERROR;
}

void CEnableFdnScreen::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, String("[EnableSimPin] ") + msg);
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos