
#include "elastos/droid/phone/CFakePhoneActivity.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CAR_INTERFACE_IMPL(CFakePhoneActivity::ButtonListener, Object, IViewOnClickListener)

ECode CFakePhoneActivity::ButtonListener::OnClick(
    /* [in] */ IView* v)
{
    if (mHost->mRadioControl == NULL) {
        Logger::E("Phone", "SimulatedRadioControl not available, abort!");
        AutoPtr<IInterface> obj;
        GetSystemService(NOTIFICATION_SERVICE, (IInterface**)&obj);
        AutoPtr<INotificationManager> nm = INotificationManager::Probe(obj);

        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText(this, String("null mRadioControl!"), IToast::LENGTH_SHORT, (IToast**)&toast);
        return toast->Show();
    }

    AutoPtr<ICharSequence> cchar;
    mHost->mPhoneNumber->GetText((ICharSequence**)&cchar);
    String str;
    cchar->ToString(&str);
    return mHost->mRadioControl->TriggerRing(str);
}

CAR_INTERFACE_IMPL(CFakePhoneActivity::MyViewOnClickListener, Object, IViewOnClickListener)

ECode CFakePhoneActivity::MyViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->mPlaceCall->RequestFocus();
}

const String CFakePhoneActivity::TAG("FakePhoneActivity");

CAR_INTERFACE_IMPL(CFakePhoneActivity, Activity, IFakePhoneActivity)

CAR_OBJECT_IMPL(CFakePhoneActivity)

ECode CFakePhoneActivity::constructor()
{
    return Activity::constructor();
}

ECode CFakePhoneActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);

    SetContentView(R.layout.fake_phone_activity);

    AutoPtr<IView> view;
    FindViewById(R.id.placeCallm, (IView**)&view);
    mPlaceCall = IButton::Probe(view);
    AutoPtr<IViewOnClickListener> listener = new ButtonListener(this);
    mPlaceCall->SetOnClickListener(listener);

    AutoPtr<IView> view2;
    FindViewById(R.id.phoneNumber, (IView**)&view2);
    mPhoneNumber = IEditText::Probe(view2);
    AutoPtr<IViewOnClickListener> listener2 = new MyViewOnClickListener(this);
    mPhoneNumber->SetOnClickListener(listener2);

    AutoPtr<IPhone> phone;
    PhoneGlobals::GetPhone((IPhone**)&phone);
    phone->GetSimulatedRadioControl((ISimulatedRadioControl**)&mRadioControl);


//     Logger::I(TAG, "- PhoneApp.getInstance(): " + PhoneGlobals.getInstance());
//     Logger::I(TAG, "- PhoneApp.getPhone(): " + PhoneGlobals.getPhone());
//     Logger::I(TAG, "- mRadioControl: " + mRadioControl);
    return NOERROR;
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos