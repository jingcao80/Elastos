#ifndef __Elastos_DevSamples_NotificationDemo_CNotificationActivity_H__
#define __Elastos_DevSamples_NotificationDemo_CNotificationActivity_H__

#include "elastos/droid/app/Activity.h"
#include "_Elastos_DevSamples_NotificationDemo_CNotificationActivity.h"

using Elastos::Droid::App::Activity;

namespace Elastos {
namespace DevSamples {
namespace NotificationDemo {

class CNotificationActivity : public Activity
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
    static const String TAG;
};

} // namespace NotificationDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_NotificationDemo_CNotificationActivity_H__
