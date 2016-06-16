
#include "elastos/droid/phone/CChangeIccPinScreen.h"

namespace Elastos {
namespace Droid {
namespace Phone {

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
        case EVENT_PIN_CHANGED:
            AsyncResult ar = (AsyncResult) msg.obj;
            mHost->HandleResult(ar);
            break;
    }

    return;
}

CAR_INTERFACE_IMPL(CChangeIccPinScreen::MyViewOnClickListener, Object, IViewOnClickListener)

ECode CChangeIccPinScreen::MyViewOnClickListenerOnClick(
    /* [in] */ IView* v)
{
    if (TO_IINTERFACE(v) == TO_IINTERFACE(mOldPin)) {
        mNewPin1->RequestFocus();
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mNewPin1)) {
        mNewPin2->RequestFocus();
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mNewPin2)) {
        mButton->RequestFocus();
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mButton)) {
        AutoPtr<IIccCard> iccCardInterface;
        mPhone->GetIccCard((IIccCard**)&iccCardInterface);
        if (iccCardInterface != NULL) {
            AutoPtr<ICharSequence> cchar;
            mOldPin->GetText((ICharSequence**)&cchar);
            String oldPin;
            cchar->ToString(&oldPin);

            AutoPtr<ICharSequence> cchar2;
            mNewPin1->GetText((ICharSequence**)&cchar2);
            String newPin1;
            cchar2->ToString(&newPin1);

            AutoPtr<ICharSequence> cchar3;
            mNewPin2->GetText((ICharSequence**)&cchar3);
            String newPin2;
            cchar3->ToString(&newPin2);

            Int32 error = ValidateNewPin(newPin1, newPin2);

            switch (error) {
                case PIN_INVALID_LENGTH:
                case PIN_MISMATCH:
                {
                    AutoPtr<ICharSequence> cchar;
                    mNewPin1->GetText((ICharSequence**)&cchar);
                    cchar->Clear();

                    AutoPtr<ICharSequence> cchar2;
                    mNewPin2->GetText((ICharSequence**)&cchar2);
                    cchar2->Clear();

                    mMismatchError->SetVisibility(IView::VISIBLE);

                    AutoPtr<IResources> r;
                    GetResources((IResources**)&r);
                    String text;

                    if (error == PIN_MISMATCH) {
                        r->GetString(R.string.mismatchPin, &text);
                    }
                    else {
                        r->GetString(R.string.invalidPin, &text);
                    }

                    AutoPtr<ICharSequence> textObj = CoreUtil::Convert(text);
                    mMismatchError->SetText(textObj);
                    break;
                }
                default:
                    AutoPtr<IMessage> callBack = Message::Obtain(mHandler, EVENT_PIN_CHANGED);

                    if (DBG) {
                        StringBuilder sb;
                        sb += "change pin attempt: old=";
                        sb += oldPin;
                        sb += ", newPin=";
                        sb += newPin1;
                        Log(sb.ToString());
                    }

                    Reset();

                    if (mChangePin2) {
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
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mPUKCode)) {
        mPUKSubmit->RequestFocus();
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mPUKSubmit)) {
        AutoPtr<ICharSequence> text;
        mPUKCode->GetText((ICharSequence**)&text);
        String str;
        text->ToString(&str);

        AutoPtr<ICharSequence> text2;
        mPUKCode->GetText((ICharSequence**)&text2);
        String str2;
        text2->ToString(&str2);

        AutoPtr<IIccCard> card;
        mPhone->GetIccCard((IIccCard**)&card);
        AutoPtr<IMessage> m = Message::Obtain(mHandler, EVENT_PIN_CHANGED);
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
    return mHost->mPUKAlert->Dismiss();
}

const String CChangeIccPinScreen::LOG_TAG = PhoneGlobals::LOG_TAG;
const Boolean CChangeIccPinScreen::DBG = FALSE;

const Int32 CChangeIccPinScreen::EVENT_PIN_CHANGED = 100;

const Int32 CChangeIccPinScreen::NO_ERROR = 0;
const Int32 CChangeIccPinScreen::PIN_MISMATCH = 1;
const Int32 CChangeIccPinScreen::PIN_INVALID_LENGTH = 2;

const Int32 CChangeIccPinScreen::MIN_PIN_LENGTH = 4;
const Int32 CChangeIccPinScreen::MAX_PIN_LENGTH = 8;

CAR_INTERFACE_IMPL(CChangeIccPinScreen, Activity, IChangeIccPinScreen)

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

ECode CChangeIccPinScreen::OnCreate()
{
    Activity::OnCreate(icicle);

    mPhone = PhoneGlobals::GetPhone();

    ResolveIntent();

    SetContentView(R.layout.change_sim_pin_screen);

    AutoPtr<IView> view1;
    FindViewById(R.id.old_pin, (IView**)&view1);
    mOldPin = IEditText::Probe(view1);
    AutoPtr<IDigitsKeyListenerHelper> helper;
    CDigitsKeyListenerHelper::AcquireSingleton((IDigitsKeyListenerHelper**)&helper);
    AutoPtr<IDigitsKeyListener> listener;
    helper->GetInstance((IDigitsKeyListener**)&listener);
    mOldPin->SetKeyListener(listener);
    mOldPin->SetMovementMethod(NULL);
    mOldPin->SetOnClickListener(mClicked);

    AutoPtr<IView> view2;
    FindViewById(R.id.new_pin1, (IView**)&view2);
    mNewPin1 = IEditText::Probe(view2);
    mNewPin1->SetKeyListener(listener);
    mNewPin1->SetMovementMethod(NULL);
    mNewPin1->SetOnClickListener(mClicked);

    AutoPtr<IView> view3;
    FindViewById(R.id.new_pin2, (IView**)&view3);
    mNewPin2 = IEditText::Probe(view3);
    mNewPin2->SetKeyListener(listener);
    mNewPin2->SetMovementMethod(NULL);
    mNewPin2->SetOnClickListener(mClicked);

    AutoPtr<IView> view4;
    FindViewById(R.id.bad_pin, (IView**)&view4);
    mBadPinError = ITextView::Probe(view4);
    AutoPtr<IView> view5;
    FindViewById(R.id.mismatch, (IView**)&view5);
    mMismatchError = ITextView::Probe(view5);

    AutoPtr<IView> view6;
    FindViewById(R.id.button, (IView**)&view6);
    mButton = IButton::Probe(view6);
    mButton->SetOnClickListener(mClicked);

    AutoPtr<IView> view7;
    FindViewById(R.id.scroll, (IView**)&view7);
    mScrollView = IScrollView::Probe(view7);

    AutoPtr<IView> view8;
    FindViewById(R.id.puk_code, (IView**)&view8);
    mPUKCode = IEditText::Probe(view8);
    mPUKCode->SetKeyListener(listener);
    mPUKCode->SetMovementMethod(NULL);
    mPUKCode->SetOnClickListener(mClicked);

    AutoPtr<IView> view9;
    FindViewById(R.id.puk_submit, (IView**)&view9);
    mPUKSubmit = IButton::Probe(view9);
    mPUKSubmit->SetOnClickListener(mClicked);

    AutoPtr<IView> view10;
    FindViewById(R.id.puk_panel, (IView**)&view10);
    mIccPUKPanel = ILinearLayout::Probe(view10);

    Int32 id = mChangePin2 ? R.string.change_pin2 : R.string.change_pin;
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
    GetIntent(IIntent**()&intent);
    intent->GetBooleanExtra(String("pin2"), mChangePin2, &mChangePin2);
}

void CChangeIccPinScreen::Reset()
{
    mScrollView->ScrollTo(0, 0);
    mBadPinError->SetVisibility(IView::GONE);
    mMismatchError->SetVisibility(IView::GONE);
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
    /* [in] */ AsyncResult ar)
{
    if (ar.exception == NULL) {
        if (DBG) Log(String("handleResult: success!"));

        if (mState == ES_PUK) {
            mScrollView->SetVisibility(IView::VISIBLE);
            mIccPUKPanel->SetVisibility(IView::GONE);
        }
        // TODO: show success feedback
        ShowConfirmation();

        AutoPtr<IRunnable> r new MyRunnable(this);
        mHandler->PostDelayed(r, 3000);
    }
    else if (ar.exception instanceof CommandException
       /*  && ((CommandException)ar.exception).getCommandError() ==
       CommandException.Error.PASSWORD_INCORRECT */ ) {
        if (mState == ES_PIN) {
            if (DBG) Log(String("handleResult: pin failed!"));
            AutoPtr<ICharSequence> csq;
            mOldPin->GetText((ICharSequence**)&csq);
            csq->Clear();
            mBadPinError->SetVisibility(IView:VISIBLE);
            CommandException ce = (CommandException) ar.exception;
            if (ce.getCommandError() == CommandException.Error.SIM_PUK2) {
                if (DBG) Log(String("handleResult: puk requested!"));
                mState = ES_PUK;
                DisplayPUKAlert();
                mScrollView->SetVisibility(IView::GONE);
                mIccPUKPanel->SetVisibility(IView::VISIBLE);
                mPUKCode->RequestFocus();
            }
        }
        else if (mState == ES_PUK) {
            //should really check to see if the error is CommandException.PASSWORD_INCORRECT...
            if (DBG) Log(String("handleResult: puk2 failed!"));
            DisplayPUKAlert();
            AutoPtr<ICharSequence> csq;
            mPUKCode->GetText((ICharSequence**)&csq);
            csq->Clear();
            mPUKCode->RequestFocus();
        }
    }
    return;
}

void CChangeIccPinScreen::DisplayPUKAlert()
{
    if (mPUKAlert == NULL) {
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
        builder->SetMessage(R.string.puk_requested);
        builder->SetCancelable(FALSE);
        builder->Show((IAlertDialog**)&mPUKAlert);
    }
    else {
        mPUKAlert->Show();
    }
    //TODO: The 3 second delay here is somewhat arbitrary, reflecting the values
    //used elsewhere for similar code.  This should get revisited with the framework
    //crew to see if there is some standard we should adhere to.
    AutoPtr<IRunnable> r = new MyRunnable2(this);
    mHandler->PostDelayed(r, 3000);
}

void CChangeIccPinScreen::ShowConfirmation()
{
    Int32 id = mChangePin2 ? R.string.pin2_changed : R.string.pin_changed;

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
    Logger::D(LOG_TAG, prefix + msg);
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos