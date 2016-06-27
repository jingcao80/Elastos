
#include "elastos/droid/teleservice/phone/CEnableFdnScreen.h"
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
            AutoPtr<AsyncResult> ar = (AsyncResult*)IObject::Probe(obj);
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
    ITextView::Probe(mHost->mPin2Field)->GetText((ICharSequence**)&text);
    if (TextUtils::IsEmpty(text)) {
        return NOERROR;
    }

    AutoPtr<IResources> resources;
    mHost->GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> text2;
    resources->GetText(Elastos::Droid::TeleService::R::string::enable_in_progress,
            (ICharSequence**)&text2);
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

    SetContentView(Elastos::Droid::TeleService::R::layout::enable_fdn_screen);
    SetupView();

    mPhone = PhoneGlobals::GetPhone();
    AutoPtr<IIccCard> card;
    mPhone->GetIccCard((IIccCard**)&card);
    card->GetIccFdnEnabled(&mEnable);
    mEnable = !mEnable;

    Int32 id = mEnable ? Elastos::Droid::TeleService::R::string::enable_fdn :
            Elastos::Droid::TeleService::R::string::disable_fdn;
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> cchar;
    resources->GetText(id, (ICharSequence**)&cchar);
    return SetTitle(cchar);
}

ECode CEnableFdnScreen::OnResume()
{
    Activity::OnResume();
    mPhone = PhoneGlobals::GetPhone();
    return NOERROR;
}

void CEnableFdnScreen::SetupView()
{
    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::TeleService::R::id::pin, (IView**)&view);
    mPin2Field = IEditText::Probe(view);

    AutoPtr<IDigitsKeyListenerHelper> helper;
    CDigitsKeyListenerHelper::AcquireSingleton((IDigitsKeyListenerHelper**)&helper);
    AutoPtr<IDigitsKeyListener> ret;
    helper->GetInstance((IDigitsKeyListener**)&ret);
    ITextView::Probe(mPin2Field)->SetKeyListener(IKeyListener::Probe(ret));
    ITextView::Probe(mPin2Field)->SetMovementMethod(NULL);
    IView::Probe(mPin2Field)->SetOnClickListener(mClicked);

    AutoPtr<IView> view2;
    FindViewById(Elastos::Droid::TeleService::R::id::pinc, (IView**)&view2);
    mPinFieldContainer = ILinearLayout::Probe(view2);

    AutoPtr<IView> view3;
    FindViewById(Elastos::Droid::TeleService::R::id::status, (IView**)&view3);
    mStatusField = ITextView::Probe(view3);
}

void CEnableFdnScreen::ShowStatus(
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

String CEnableFdnScreen::GetPin2()
{
    AutoPtr<ICharSequence> cchar;
    ITextView::Probe(mPin2Field)->GetText((ICharSequence**)&cchar);
    String str;
    cchar->ToString(&str);
    return str;
}

void CEnableFdnScreen::EnableFdn()
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> callback;
    helper->Obtain(mHandler, ENABLE_FDN_COMPLETE, (IMessage**)&callback);

    AutoPtr<IIccCard> card;
    mPhone->GetIccCard((IIccCard**)&card);
    card->SetIccFdnEnabled(mEnable, GetPin2(), callback);

    if (DBG) Log(String("enableFdn: please wait..."));
}

void CEnableFdnScreen::HandleResult(
    /* [in] */ AsyncResult* ar)
{
    if (ar->mException == NULL) {
        if (DBG) Log(String("handleResult: success!"));
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr<ICharSequence> cchar;
        resources->GetText(mEnable ? Elastos::Droid::TeleService::R::string::enable_fdn_ok :
                Elastos::Droid::TeleService::R::string::disable_fdn_ok, (ICharSequence**)&cchar);
        ShowStatus(cchar);
    }
    else if (ICommandException::Probe(ar->mException) != NULL
            /* && ((CommandException)ar.exception).getCommandError() ==
                CommandException.Error.GENERIC_FAILURE */ ) {
        if (DBG) Log(String("handleResult: failed!"));
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr<ICharSequence> cchar;
        resources->GetText(Elastos::Droid::TeleService::R::string::pin_failed,
                (ICharSequence**)&cchar);
        ShowStatus(cchar);
    }

    AutoPtr<IRunnable> r;
    Boolean res;
    mHandler->PostDelayed(r, 3000, &res);
}

void CEnableFdnScreen::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, String("[EnableSimPin] ") + msg);
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos