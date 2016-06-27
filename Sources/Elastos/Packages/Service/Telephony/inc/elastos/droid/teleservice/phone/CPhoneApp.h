#ifndef  __ELASTOS_DROID_PHONE_CPHONEAPP_H__
#define  __ELASTOS_DROID_PHONE_CPHONEAPP_H__

#include "_Elastos_Droid_TeleService_Phone_CPhoneApp.h"
#include "elastos/droid/app/Application.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::App::Application;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Top-level Application class for the Phone app.
 */
CarClass(CPhoneApp)
    , public Application
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate();

public:
    // AutoPtr<PhoneGlobals> mPhoneGlobals;
    // AutoPtr<TelephonyGlobals> mTelephonyGlobals;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CPHONEAPP_H__