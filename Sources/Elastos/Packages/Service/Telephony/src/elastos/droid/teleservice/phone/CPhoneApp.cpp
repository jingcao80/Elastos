
#include "elastos/droid/teleservice/phone/CPhoneApp.h"
#include "elastos/droid/app/Application.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::App::Application;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_OBJECT_IMPL(CPhoneApp)

ECode CPhoneApp::constructor()
{
    return Application::constructor();
}

ECode CPhoneApp::OnCreate()
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 myUserId = 0;
    helper->GetMyUserId(&myUserId);

    assert(0 && "need class PhoneGlobals && TelephonyGlobals");
    // if (myUserId == 0) {
    //     // We are running as the primary user, so should bring up the
    //     // global phone state.
    //     mPhoneGlobals = new PhoneGlobals((IContext*)this);
    //     mPhoneGlobals->OnCreate();

    //     mTelephonyGlobals = new TelephonyGlobals((IContext*)this);
    //     return mTelephonyGlobals->OnCreate();
    // }
    return NOERROR;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos