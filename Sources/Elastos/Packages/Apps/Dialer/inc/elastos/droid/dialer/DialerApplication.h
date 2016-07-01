#ifndef __ELASTOS_DROID_DIALER_DIALERAPPLICATION_H__
#define __ELASTOS_DROID_DIALER_DIALERAPPLICATION_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/app/Application.h"

using Elastos::Droid::App::Application;

namespace Elastos {
namespace Droid {
namespace Dialer {

class DialerApplication
    : public Application
    , public IDialerApplication
{
public:
    CAR_INTERFACE_DECL();

    //@Override
    CARAPI OnCreate();

    //@Override
    CARAPI GetSystemService(
        /* [in] */ const String& name,
        /* [out] */ IInterface** object);

private:
    // AutoPtr<IContactPhotoManager> mContactPhotoManager;
};

} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALERAPPLICATION_H__
