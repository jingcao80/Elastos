
#include "elastos/droid/teleservice/phone/CChangeIccPinScreen.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "R.h"
#include "Elastos.Droid.Text.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Internal::Telephony::IIccCard;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Text::Method::CDigitsKeyListenerHelper;
using Elastos::Droid::Text::Method::IDigitsKeyListener;
using Elastos::Droid::Text::Method::IDigitsKeyListenerHelper;
using Elastos::Droid::Text::Method::IKeyListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

const String CChangeIccPinScreen::TAG = PhoneGlobals::TAG;
const Boolean CChangeIccPinScreen::DBG = FALSE;
const Int32 CChangeIccPinScreen::EVENT_PIN_CHANGED = 100;
const Int32 CChangeIccPinScreen::NO_ERROR = 0;
const Int32 CChangeIccPinScreen::PIN_MISMATCH = 1;
const Int32 CChangeIccPinScreen::PIN_INVALID_LENGTH = 2;
const Int32 CChangeIccPinScreen::MIN_PIN_LENGTH = 4;
const Int32 CChangeIccPinScreen::MAX_PIN_LENGTH = 8;

CChangeIccPinScreen::MyHandler::MyHandler(
    /* [in] */ CChangeIccPinScreen* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CChangeIccPinScreen::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_PIN_CHANGED: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AsyncResult* ar = (AsyncResult*)IAsyncResult::Probe(obj);
            mHost->HandleResult(ar);
            break;
        }
    }

    return NOERROR;
}

CAR_INTERFACE_IMPL(CChangeIccPinScreen::MyViewOnClickListener, Object, IViewOnClickListener)
ECode CChangeIccPinScreen::MyViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    if (TO_IINTERFACE(v) == TO_IINTERFACE(mHost->mOldPin)) {
        Boolean tmp = FALSE;
        IView::Probe(mHost->mNewPin1)->RequestFocus(&tmp);
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mHost->mNewPin1)) {
        Boolean tmp = FALSE;
        IView::Probe(mHost->mNewPin2)->RequestFocus(&tmp);
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mHost->mNewPin2)) {
        Boolean tmp = FALSE;
        IView::Probe(mHost->mButton)->RequestFocus(&tmp);
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mHost->mButton)) {
        AutoPtr<IIccCard> iccCardInterface;
        mHost->mPhone->GetIccCard((IIccCard**)&iccCardInterface);
        if (iccCardInterface != NULL) {
            AutoPtr<ICharSequence> cchar;
            ITextView::Probe(mHost->mOldPin)->GetText((ICharSequence**)&cchar);
            String oldPin;
            cchar->ToString(&oldPin);

            AutoPtr<ICharSequence> cchar2;
            ITextView::Probe(mHost->mNewPin1)->GetText((ICharSequence**)&cchar2);
            String newPin1;
            cchar2->ToString(&newPin1);

            AutoPtr<ICharSequence> cchar3;
            ITextView::Probe(mHost->mNewPin2)->GetText((ICharSequence**)&cchar3);
            String newPin2;
            cchar3->ToString(&newPin2);

            Int32 error = mHost->ValidateNewPin(newPin1, newPin2);

            switch (error) {
                case PIN_INVALID_LENGTH:
                case PIN_MISMATCH:
                {
                    AutoPtr<ICharSequence> cchar;
                    ITextView::Probe(mHost->mNewPin1)->GetText((ICharSequence**)&cchar);

                    AutoPtr<ICharSequence> cchar2;
                    ITextView::Probe(mHost->mNewPin2)->GetText((ICharSequence**)&cchar2);

                    IView::Probe(mHost->mMismatchError)->SetVisibility(IView::VISIBLE);

                    AutoPtr<IResources> r;
                    mHost->GetResources((IResources**)&r);
                    String text;

                    if (error == PIN_MISMATCH) {
                        r->GetString(Elastos::Droid::TeleService::R::string::mismatchPin, &text);
                    }
                    else {
                        r->GetString(Elastos::Droid::TeleService::R::string::invalidPin, &text);
                    }

                    AutoPtr<ICharSequence> textObj = CoreUtils::Convert(text);
                    mHost->mMismatchError->SetText(textObj);
                    break;
                }
                default: {
                    AutoPtr<IMessageHelper> helper;
                    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
                    AutoPtr<IMessage> callBack;
                    helper->Obtain(mHost->mHandler, EVENT_PIN_CHANGED, (IMessage**)&callBack);

                    if (DBG) {
                        StringBuilder sb;
                        sb += "change pin attempt: old=";
                        sb += oldPin;
                        sb += ", newPin=";
                        sb += newPin1;
                        mHost->Log(sb.ToString());
                    }

                    mHost->Reset();

                    if (mHost->mChangePin2) {
                        iccCardInterface->ChangeIccFdnPassword(oldPin,
                                newPin1, callBack);
                    }
                    else {
                        iccCardInterface->ChangeIccLockPassword(oldPin,
                                newPin1, callBack);
                    }

                    // TODO: show progress panel
                }
            }
        }
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mHost->mPUKCode)) {
        Boolean tmp = FALSE;
        IView::Probe(mHost->mPUKSubmit)->RequestFocus(&tmp);
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mHost->mPUKSubmit)) {
        AutoPtr<ICharSequence> text;
        ITextView::Probe(mHost->mPUKCode)->GetText((ICharSequence**)&text);
        String str;
        text->ToString(&str);

        AutoPtr<ICharSequence> text2;
        ITextView::Probe(mHost->mPUKCode)->GetText((ICharSequence**)&text2);
        String str2;
        text2->ToString(&str2);

        AutoPtr<IIccCard> card;
        mHost->mPhone->GetIccCard((IIccCard**)&card);

        AutoPtr<IMessageHelper> helper;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
        AutoPtr<IMessage> m;
        helper->Obtain(mHost->mHandler, EVENT_PIN_CHANGED, (IMessage**)&m);
        card->SupplyPuk2(str, str2, m);
    }
    return NOERROR;
}

ECode CChangeIccPinScreen::MyRunnable::Run()
{
    return mHost->Finish();
}

ECode CChangeIccPinScreen::MyRunnable2::Run()
{
    return IDialogInterface::Probe(mHost->mPUKAlert)->Dismiss();
}

CAR_OBJECT_IMPL(CChangeIccPinScreen)

CChangeIccPinScreen::CChangeIccPinScreen()
    : mState(ES_PIN)
    , mChangePin2(FALSE)
{
    mHandler = new MyHandler(this);

    mClicked = new MyViewOnClickListener(this);
}

ECode CChangeIccPinScreen::constructor()
{
    return Activity::constructor();
}

ECode CChangeIccPinScreen::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);

    mPhone = PhoneGlobals::GetPhone();

    ResolveIntent();

    SetContentView(Elastos::Droid::TeleService::R::layout::change_sim_pin_screen);

    AutoPtr<IView> view1;
    FindViewById(Elastos::Droid::TeleService::R::id::old_pin, (IView**)&view1);
    mOldPin = IEditText::Probe(view1);
    AutoPtr<IDigitsKeyListenerHelper> helper;
    CDigitsKeyListenerHelper::AcquireSingleton((IDigitsKeyListenerHelper**)&helper);
    AutoPtr<IDigitsKeyListener> listener;
    helper->GetInstance((IDigitsKeyListener**)&listener);
    ITextView::Probe(mOldPin)->SetKeyListener(IKeyListener::Probe(listener));
    ITextView::Probe(mOldPin)->SetMovementMethod(NULL);
    IView::Probe(mOldPin)->SetOnClickListener(mClicked);

    AutoPtr<IView> view2;
    FindViewById(Elastos::Droid::TeleService::R::id::new_pin1, (IView**)&view2);
    mNewPin1 = IEditText::Probe(view2);
    ITextView::Probe(mNewPin1)->SetKeyListener(IKeyListener::Probe(listener));
    ITextView::Probe(mNewPin1)->SetMovementMethod(NULL);
    IView::Probe(mNewPin1)->SetOnClickListener(mClicked);

    AutoPtr<IView> view3;
    FindViewById(Elastos::Droid::TeleService::R::id::new_pin2, (IView**)&view3);
    mNewPin2 = IEditText::Probe(view3);
    ITextView::Probe(mNewPin2)->SetKeyListener(IKeyListener::Probe(listener));
    ITextView::Probe(mNewPin2)->SetMovementMethod(NULL);
    IView::Probe(mNewPin2)->SetOnClickListener(mClicked);

    AutoPtr<IView> view4;
    FindViewById(Elastos::Droid::TeleService::R::id::bad_pin, (IView**)&view4);
    mBadPinError = ITextView::Probe(view4);
    AutoPtr<IView> view5;
    FindViewById(Elastos::Droid::TeleService::R::id::mismatch, (IView**)&view5);
    mMismatchError = ITextView::Probe(view5);

    AutoPtr<IView> view6;
    FindViewById(Elastos::Droid::TeleService::R::id::button, (IView**)&view6);
    mButton = IButton::Probe(view6);
    IView::Probe(mButton)->SetOnClickListener(mClicked);

    AutoPtr<IView> view7;
    FindViewById(Elastos::Droid::TeleService::R::id::scroll, (IView**)&view7);
    mScrollView = IScrollView::Probe(view7);

    AutoPtr<IView> view8;
    FindViewById(Elastos::Droid::TeleService::R::id::puk_code, (IView**)&view8);
    mPUKCode = IEditText::Probe(view8);
    ITextView::Probe(mPUKCode)->SetKeyListener(IKeyListener::Probe(listener));
    ITextView::Probe(mPUKCode)->SetMovementMethod(NULL);
    IView::Probe(mPUKCode)->SetOnClickListener(mClicked);

    AutoPtr<IView> view9;
    FindViewById(Elastos::Droid::TeleService::R::id::puk_submit, (IView**)&view9);
    mPUKSubmit = IButton::Probe(view9);
    IView::Probe(mPUKSubmit)->SetOnClickListener(mClicked);

    AutoPtr<IView> view10;
    FindViewById(Elastos::Droid::TeleService::R::id::puk_panel, (IView**)&view10);
    mIccPUKPanel = ILinearLayout::Probe(view10);

    Int32 id = mChangePin2 ? Elastos::Droid::TeleService::R::string::change_pin2
            : Elastos::Droid::TeleService::R::string::change_pin;
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> csq;
    resources->GetText(id, (ICharSequence**)&csq);
    SetTitle(csq);

    mState = ES_PIN;
    return NOERROR;
}

void CChangeIccPinScreen::ResolveIntent()
{
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    intent->GetBooleanExtra(String("pin2"), mChangePin2, &mChangePin2);
}

void CChangeIccPinScreen::Reset()
{
    IView::Probe(mScrollView)->ScrollTo(0, 0);
    IView::Probe(mBadPinError)->SetVisibility(IView::GONE);
    IView::Probe(mMismatchError)->SetVisibility(IView::GONE);
}

Int32 CChangeIccPinScreen::ValidateNewPin(
    /* [in] */ const String& p1,
    /* [in] */ const String& p2)
{
    if (p1.IsNull()) {
        return PIN_INVALID_LENGTH;
    }

    if (!p1.Equals(p2)) {
        return PIN_MISMATCH;
    }

    Int32 len1 = p1.GetLength();

    if (len1 < MIN_PIN_LENGTH || len1 > MAX_PIN_LENGTH) {
        return PIN_INVALID_LENGTH;
    }

    return NO_ERROR;
}

void CChangeIccPinScreen::HandleResult(
    /* [in] */ AsyncResult* ar)
{
    if (ar->mException == NULL) {
        if (DBG) Log(String("handleResult: success!"));

        if (mState == ES_PUK) {
            IView::Probe(mScrollView)->SetVisibility(IView::VISIBLE);
            IView::Probe(mIccPUKPanel)->SetVisibility(IView::GONE);
        }
        // TODO: show success feedback
        ShowConfirmation();

        AutoPtr<IRunnable> r = new MyRunnable(this);
        Boolean tmp = FALSE;
        mHandler->PostDelayed(r, 3000, &tmp);
    }
    assert(0 && "TODO CommandException");
    // else if (ar.exception instanceof CommandException
    //    /*  && ((CommandException)ar.exception).getCommandError() ==
    //    CommandException.Error.PASSWORD_INCORRECT */ ) {
    //     if (mState == ES_PIN) {
    //         if (DBG) Log(String("handleResult: pin failed!"));
    //         AutoPtr<ICharSequence> csq;
    //         mOldPin->GetText((ICharSequence**)&csq);
    //         csq->Clear();
    //         IView::Probe(mBadPinError)->SetVisibility(IView:VISIBLE);
    //         CommandException ce = (CommandException) ar.exception;
    //         if (ce.getCommandError() == CommandException.Error.SIM_PUK2) {
    //             if (DBG) Log(String("handleResult: puk requested!"));
    //             mState = ES_PUK;
    //             DisplayPUKAlert();
    //             IView::Probe(mScrollView)->SetVisibility(IView::GONE);
    //             IView::Probe(mIccPUKPanel)->SetVisibility(IView::VISIBLE);
    //             IView::Probe(mPUKCode)->RequestFocus();
    //         }
    //     }
    //     else if (mState == ES_PUK) {
    //         //should really check to see if the error is CommandException.PASSWORD_INCORRECT...
    //         if (DBG) Log(String("handleResult: puk2 failed!"));
    //         DisplayPUKAlert();
    //         AutoPtr<ICharSequence> csq;
    //         mPUKCode->GetText((ICharSequence**)&csq);
    //         csq->Clear();
    //         IView::Probe(mPUKCode)->RequestFocus();
    //     }
    // }
    return;
}

void CChangeIccPinScreen::DisplayPUKAlert()
{
    if (mPUKAlert == NULL) {
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
        builder->SetMessage(Elastos::Droid::TeleService::R::string::puk_requested);
        builder->SetCancelable(FALSE);
        builder->Show((IAlertDialog**)&mPUKAlert);
    }
    else {
        IDialog::Probe(mPUKAlert)->Show();
    }
    //TODO: The 3 second delay here is somewhat arbitrary, reflecting the values
    //used elsewhere for similar code.  This should get revisited with the framework
    //crew to see if there is some standard we should adhere to.
    AutoPtr<IRunnable> r = new MyRunnable2(this);
    Boolean tmp = FALSE;
    mHandler->PostDelayed(r, 3000, &tmp);
}

void CChangeIccPinScreen::ShowConfirmation()
{
    Int32 id = mChangePin2 ? Elastos::Droid::TeleService::R::string::pin2_changed : Elastos::Droid::TeleService::R::string::pin_changed;

    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    AutoPtr<IToast> toast;
    helper->MakeText(this, id, IToast::LENGTH_SHORT, (IToast**)&toast);
    toast->Show();
}

void CChangeIccPinScreen::Log(
    /* [in] */ const String& msg)
{
    String prefix = mChangePin2 ? String("[ChgPin2]") : String("[ChgPin]");
    Logger::D(TAG, prefix + msg);
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos