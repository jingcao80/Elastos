
#include "elastos/droid/teleservice/phone/CEnableIccPinScreen.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Text.h"
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IIccCard;
using Elastos::Droid::Internal::Telephony::ICommandException;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Method::IKeyListener;
using Elastos::Droid::Text::Method::IDigitsKeyListener;
using Elastos::Droid::Text::Method::IDigitsKeyListenerHelper;
using Elastos::Droid::Text::Method::CDigitsKeyListenerHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
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
            AutoPtr<AsyncResult> ar = (AsyncResult*)IAsyncResult::Probe(obj);
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
    ITextView::Probe(mHost->mPinField)->GetText((ICharSequence**)&text);
    if (TextUtils::IsEmpty(text)) {
        return NOERROR;
    }

    AutoPtr<IResources> resources;
    mHost->GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> text2;
    resources->GetText(Elastos::Droid::TeleService::R::string::enable_in_progress,
            (ICharSequence**)&text2);
    mHost->ShowStatus(text2);

    mHost->EnableIccPin();
    return NOERROR;
}

ECode CEnableIccPinScreen::MyRunnable::Run()
{
    return mHost->Finish();
}

const String CEnableIccPinScreen::TAG("CEnableIccPinScreen");// = PhoneGlobals.TAG;

const Boolean CEnableIccPinScreen::DBG = FALSE;

CAR_OBJECT_IMPL(CEnableIccPinScreen)

CEnableIccPinScreen::CEnableIccPinScreen()
    : mEnable(FALSE)
{
    mHandler = new MyHandler(this);

    mClicked = new MyViewOnClickListener(this);
}

ECode CEnableIccPinScreen::constructor()
{
    return Activity::constructor();
}

ECode CEnableIccPinScreen::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);

    SetContentView(Elastos::Droid::TeleService::R::layout::enable_sim_pin_screen);
    SetupView();

    mPhone = PhoneGlobals::GetPhone();
    AutoPtr<IIccCard> card;
    mPhone->GetIccCard((IIccCard**)&card);
    card->GetIccLockEnabled(&mEnable);
    mEnable = !mEnable;

    Int32 id = mEnable ? Elastos::Droid::TeleService::R::string::enable_sim_pin :
            Elastos::Droid::TeleService::R::string::disable_sim_pin;
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> cchar;
    resources->GetText(id, (ICharSequence**)&cchar);
    return SetTitle(cchar);
}

void CEnableIccPinScreen::SetupView()
{
    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::TeleService::R::id::pin, (IView**)&view);
    mPinField = IEditText::Probe(view);

    AutoPtr<IDigitsKeyListenerHelper> helper;
    CDigitsKeyListenerHelper::AcquireSingleton((IDigitsKeyListenerHelper**)&helper);
    AutoPtr<IDigitsKeyListener> ret;
    helper->GetInstance((IDigitsKeyListener**)&ret);
    ITextView::Probe(mPinField)->SetKeyListener(IKeyListener::Probe(ret));
    ITextView::Probe(mPinField)->SetMovementMethod(NULL);
    IView::Probe(mPinField)->SetOnClickListener(mClicked);

    AutoPtr<IView> view2;
    FindViewById(Elastos::Droid::TeleService::R::id::pinc, (IView**)&view2);
    mPinFieldContainer = ILinearLayout::Probe(view2);

    AutoPtr<IView> view3;
    FindViewById(Elastos::Droid::TeleService::R::id::status, (IView**)&view3);
    mStatusField = ITextView::Probe(view3);
}

void CEnableIccPinScreen::ShowStatus(
    /* [in] */ ICharSequence* statusMsg)
{
    if (statusMsg != NULL) {
        mStatusField->SetText(statusMsg);
        IView::Probe(mStatusField)->SetVisibility(IView::VISIBLE);
        IView::Probe(mPinFieldContainer)->SetVisibility(IView::GONE);
    }
    else {
        IView::Probe(mPinFieldContainer)->SetVisibility(IView::VISIBLE);
        IView::Probe(mStatusField)->SetVisibility(IView::GONE);
    }
}

String CEnableIccPinScreen::GetPin()
{
    AutoPtr<ICharSequence> cchar;
    ITextView::Probe(mPinField)->GetText((ICharSequence**)&cchar);
    String str;
    cchar->ToString(&str);
    return str;
}

void CEnableIccPinScreen::EnableIccPin()
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> callback;
    helper->Obtain(mHandler, ENABLE_ICC_PIN_COMPLETE, (IMessage**)&callback);

    if (DBG) Log(String("enableIccPin:"));

    AutoPtr<IIccCard> card;
    mPhone->GetIccCard((IIccCard**)&card);
    card->SetIccLockEnabled(mEnable, GetPin(), callback);

    if (DBG) Log(String("enableIccPin: please wait..."));
}

void CEnableIccPinScreen::HandleResult(
    /* [in] */ AsyncResult* ar)
{
    if (ar->mException == NULL) {
        if (DBG) Log(String("handleResult: success!"));
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr<ICharSequence> cchar;
        resources->GetText(mEnable ? Elastos::Droid::TeleService::R::string::enable_pin_ok :
                Elastos::Droid::TeleService::R::string::disable_pin_ok, (ICharSequence**)&cchar);
        ShowStatus(cchar);
    }
    else if (ICommandException::Probe(ar->mException) != NULL
            /* && ((CommandException)ar.exception).getCommandError() ==
                CommandException.Error.GENERIC_FAILURE */ ) {
        if (DBG) Log(String("handleResult: failed!"));
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr<ICharSequence> cchar;
        resources->GetText(Elastos::Droid::TeleService::R::string::pin_failed, (ICharSequence**)&cchar);
        ShowStatus(cchar);
    }

    AutoPtr<IRunnable> r = new MyRunnable(this);
    Boolean res;
    mHandler->PostDelayed(r, 3000, &res);
}

void CEnableIccPinScreen::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, String("[EnableIccPin] ") + msg);
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos