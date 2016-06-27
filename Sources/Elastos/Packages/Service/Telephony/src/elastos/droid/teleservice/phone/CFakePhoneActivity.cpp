
#include "elastos/droid/teleservice/phone/CFakePhoneActivity.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "R.h"

using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_INTERFACE_IMPL(CFakePhoneActivity::ButtonListener, Object, IViewOnClickListener)

ECode CFakePhoneActivity::ButtonListener::OnClick(
    /* [in] */ IView* v)
{
    assert(0);
    // if (mHost->mRadioControl == NULL) {
    //     Logger::E("Phone", "SimulatedRadioControl not available, abort!");
    //     AutoPtr<IInterface> obj;
    //     GetSystemService(NOTIFICATION_SERVICE, (IInterface**)&obj);
    //     AutoPtr<INotificationManager> nm = INotificationManager::Probe(obj);

    //     CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    //     AutoPtr<IToast> toast;
    //     helper->MakeText(this, String("null mRadioControl!"), IToast::LENGTH_SHORT, (IToast**)&toast);
    //     return toast->Show();
    // }

    AutoPtr<ICharSequence> cchar;
    ITextView::Probe(mHost->mPhoneNumber)->GetText((ICharSequence**)&cchar);
    String str;
    cchar->ToString(&str);
    assert(0);
    //return mHost->mRadioControl->TriggerRing(str);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CFakePhoneActivity::MyViewOnClickListener, Object, IViewOnClickListener)

ECode CFakePhoneActivity::MyViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    Boolean res;
    return IView::Probe(mHost->mPlaceCall)->RequestFocus(&res);
}

const String CFakePhoneActivity::TAG("FakePhoneActivity");

CAR_OBJECT_IMPL(CFakePhoneActivity)

ECode CFakePhoneActivity::constructor()
{
    return Activity::constructor();
}

ECode CFakePhoneActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);

    SetContentView(Elastos::Droid::TeleService::R::layout::fake_phone_activity);

    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::TeleService::R::id::placeCall, (IView**)&view);
    mPlaceCall = IButton::Probe(view);
    AutoPtr<IViewOnClickListener> listener = new ButtonListener(this);
    IView::Probe(mPlaceCall)->SetOnClickListener(listener);

    AutoPtr<IView> view2;
    FindViewById(Elastos::Droid::TeleService::R::id::phoneNumber, (IView**)&view2);
    mPhoneNumber = IEditText::Probe(view2);
    AutoPtr<IViewOnClickListener> listener2 = new MyViewOnClickListener(this);
    IView::Probe(mPhoneNumber)->SetOnClickListener(listener2);

    AutoPtr<IPhone> phone = PhoneGlobals::GetPhone();
    assert(0);
    //phone->GetSimulatedRadioControl((ISimulatedRadioControl**)&mRadioControl);

//     Logger::I(TAG, "- PhoneApp.getInstance(): " + PhoneGlobals.getInstance());
//     Logger::I(TAG, "- PhoneApp.getPhone(): " + PhoneGlobals.getPhone());
//     Logger::I(TAG, "- mRadioControl: " + mRadioControl);
    return NOERROR;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos