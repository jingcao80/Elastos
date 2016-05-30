#ifndef __Elastos_DevSamples_ProgressBarDemo_CACTIVITYONE_H__
#define __Elastos_DevSamples_ProgressBarDemo_CACTIVITYONE_H__

#include "_Elastos_DevSamples_ProgressBarDemo_CActivityOne.h"
#include <elastos/droid/app/Activity.h>
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Widget::IProgressBar;

namespace Elastos {
namespace DevSamples {
namespace ProgressBarDemo {

CarClass(CActivityOne)
    , public Activity
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();
};

} // namespace ProgressBarDemo
} // namespace DevSamples
} // namespace Elastos

#endif //__Elastos_DevSamples_ProgressBarDemo_CACTIVITYONE_H__