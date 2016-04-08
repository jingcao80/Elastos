
#ifndef __CACTIONACTIVITYCAST_H__
#define __CACTIONACTIVITYCAST_H__

#include "elastos/droid/app/Activity.h"
#include "_CActionActivityCast.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AlarmManagerDemo {

class CActionActivityCast : public Activity
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

private:
    static Int32 mNum;
};

} // namespace AlarmManagerDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIONACTIVITYCAST_H__
