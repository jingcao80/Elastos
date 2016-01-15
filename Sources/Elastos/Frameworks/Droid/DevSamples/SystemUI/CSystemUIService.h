
#ifndef __CSYSTEMUISERVICE_H__
#define __CSYSTEMUISERVICE_H__

#include "_CSystemUIService.h"
#include "elastos/droid/app/Service.h"

using Elastos::Droid::SystemUI::ISystemUI;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SystemUI {

class CSystemUIService : public Elastos::Droid::App::Service
{
public:
    CSystemUIService();

    PInterface Probe(
        /* [in] */ REIID riid);

    CARAPI OnCreate();

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    /**
     * Nobody binds to us.
     */
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

private:
    static const String TAG;

    AutoPtr<ArrayOf<ISystemUI*> > mServices;
};

} // SystemUI
} // DevSamples
} // Droid
} // Elastos

#endif //__CSYSTEMUISERVICE_H__
