
#ifndef __Elastos_DevSamples_DrawPathDemo_CActivityOne_H__
#define __Elastos_DevSamples_DrawPathDemo_CActivityOne_H__

#include <elastos/droid/app/Activity.h>
#include "_Elastos_DevSamples_DrawPathDemo_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace DevSamples {
namespace DrawPathDemo {

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

private:
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

private:
};

} // namespace DrawPathDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_DrawPathDemo_CActivityOne_H__
