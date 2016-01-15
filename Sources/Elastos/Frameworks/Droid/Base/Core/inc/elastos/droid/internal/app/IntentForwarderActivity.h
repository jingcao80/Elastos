
#ifndef __ELASTOS_DROID_INTERNAL_APP_INTENTFORWARDERACTIVITY_H__
#define __ELASTOS_DROID_INTERNAL_APP_INTENTFORWARDERACTIVITY_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class IntentForwarderActivity
    : public Activity
    , public IIntentForwarderActivity
{
public:
    CAR_INTERFACE_DECL()

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    /**
     * Returns the managed profile for this device or null if there is no managed
     * profile.
     *
     * TODO: Remove the assumption that there is only one managed profile
     * on the device.
     */
    CARAPI_(AutoPtr<IUserHandle>) GetManagedProfile();

private:
    static const String TAG;
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_INTENTFORWARDERACTIVITY_H__
