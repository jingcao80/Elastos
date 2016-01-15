
#ifndef __ELASTOS_DROID_INTERNAL_APP_DISABLECARMODEACTIVITY_H__
#define __ELASTOS_DROID_INTERNAL_APP_DISABLECARMODEACTIVITY_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class DisableCarModeActivity
    : public Activity
    , public IDisableCarModeActivity
{
public:
    CAR_INTERFACE_DECL()

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    static const String TAG;
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_DISABLECARMODEACTIVITY_H__
