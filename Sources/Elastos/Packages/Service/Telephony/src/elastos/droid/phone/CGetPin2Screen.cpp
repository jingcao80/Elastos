
#include "elastos/droid/phone/CGetPin2Screen.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CAR_INTERFACE_IMPL_2(CGetPin2Screen:MyViewOnClickListener, Object, IViewOnClickListener)

CGetPin2Screen::MyViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    String str;
    mPin2Field->GetText(&str);
    if (TextUtils::IsEmpty(str)) {
        return NOERROR;
    }

    mHost->ReturnResult();
    return NOERROR;
}

const String CGetPin2Screen::LOG_TAG("CGetPin2Screen");// = PhoneGlobals.LOG_TAG;

CAR_INTERFACE_IMPL_2(CGetPin2Screen, Activity, IGetPin2Screen, ITextViewOnEditorActionListener)

CAR_OBJECT_IMPL(CGetPin2Screen)

CGetPin2Screen::CGetPin2Screen()
{
    mClicked = new MyViewOnClickListener(this);
}

ECode CGetPin2Screen::constructor()
{
    return Activity::constructor();
}

ECode CGetPin2Screen::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);

    SetContentView(R.layout.get_pin2_screen);

    AutoPtr<IView> view;
    FindViewById(R.id.pin, (IView**)&view);
    mPin2Field = IEditText::Probe(view);

    AutoPtr<IDigitsKeyListenerHelper> helper;
    CDigitsKeyListenerHelper::AcquireSingleton((IDigitsKeyListenerHelper**)&helper);
    AutoPtr<IDigitsKeyListener> listener;
    helper->GetInstance((IDigitsKeyListener**)&listener);
    mPin2Field->SetKeyListener(listener);
    mPin2Field->SetMovementMethod(NULL);
    mPin2Field->SetOnEditorActionListener(this);
    mPin2Field->SetInputType(IInputType:TYPE_CLASS_NUMBER | IInputType::TYPE_NUMBER_VARIATION_PASSWORD);

    AutoPtr<IView> view2;
    FindViewById(R.id.ok, (IView**)&view2);
    mOkButton = IButton::Probe(view2);
    return mOkButton->SetOnClickListener(mClicked);
}

String CGetPin2Screen::GetPin2()
{
    AutoPtr<ICharSequence> cchar;
    mPin2Field->GetText((ICharSequence**)&cchar);
    String str;
    cchar->ToString(&str);
    return str;
}

void CGetPin2Screen::ReturnResult()
{
    AutoPtr<IBundle> map;
    CBundle::New((IBundle**)&map);
    map->PutString(String("pin2"), GetPin2());

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IUri> uri;
    intent->GetData((IUri**)&uri);

    AutoPtr<IIntent> action;
    CIntent::New((IIntent**)&action);

    if (uri != NULL) {
        String str;
        uri->ToString(&str);
        action->SetAction(str);
    }

    action->PutExtras(map);
    SetResult(RESULT_OK, action);
    Finish();
}

ECode CGetPin2Screen::OnEditorAction(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (actionId == iEditorInfo::IME_ACTION_DONE) {
        mOkButton->PerformClick();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

void CGetPin2Screen::Log(
    /* [in] */ const String& msg)
{
    String sb;
    sb += "[GetPin2] ";
    sb += msg;
    Logger::D(LOG_TAG, sb.ToString());
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos