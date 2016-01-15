#ifndef __ELASTOS_DROID_DEVSAMPLES_MYFIRSTAPP_CMAINACTIVITY_H__
#define __ELASTOS_DROID_DEVSAMPLES_MYFIRSTAPP_CMAINACTIVITY_H__

#include "app/Activity.h"
#include "_CMainActivity.h"

using Elastos::Droid::App::Activity;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace MyFirstApp {

class CMainActivity : public Activity
{
protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnRestart();

    CARAPI OnPause();

    CARAPI OnResume();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    static const String TAG;
};

} //namespace Myfirstapp
} //namespace DevSamples
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_DEVSAMPLES_MYFIRSTAPP_CMAINACTIVITY_H__
