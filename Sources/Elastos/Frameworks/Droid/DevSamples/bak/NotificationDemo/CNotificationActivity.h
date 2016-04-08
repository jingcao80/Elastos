#ifndef __CNOTIFICATIONACTIVITY_H__
#define __CNOTIFICATIONACTIVITY_H__

#include "elastos/droid/app/Activity.h"
#include "_CNotificationActivity.h"

using Elastos::Droid::App::Activity;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace NotificationDemo {

class CNotificationActivity : public Activity
{
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
};

} // namespace NotificationDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CNOTIFICATIONACTIVITY_H__
