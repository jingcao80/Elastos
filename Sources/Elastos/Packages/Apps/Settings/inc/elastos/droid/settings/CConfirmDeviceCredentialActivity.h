
#ifndef __ELASTOS_DROID_SETTINGS_CCONFIRMDEVICECREDENTIALACTIVITY_H__
#define __ELASTOS_DROID_SETTINGS_CCONFIRMDEVICECREDENTIALACTIVITY_H__

#include "_Elastos_Droid_Settings_CConfirmDeviceCredentialActivity.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * Launch this when you want to confirm the user is present by asking them to enter their
 * PIN/password/pattern.
 */
CarClass(CConfirmDeviceCredentialActivity)
    , public Activity
{
public:
    CAR_OBJECT_DECL()

    CConfirmDeviceCredentialActivity();

    virtual ~CConfirmDeviceCredentialActivity();

    CARAPI constructor();

    static CARAPI_(AutoPtr<IIntent>) CreateIntent(
        /* [in] */ ICharSequence* title,
        /* [in] */ ICharSequence* details);

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

protected:
    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

public:
    static const String TAG;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCONFIRMDEVICECREDENTIALACTIVITY_H__