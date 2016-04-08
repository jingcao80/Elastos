
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Hardware::ISerialManager;
using Elastos::Droid::Hardware::IISerialManager;
using Elastos::Droid::Hardware::ISerialPort;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SerialManagerDemo {

class CActivityOne : public Activity
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

public:
    AutoPtr<ISerialManager> mSm;
    AutoPtr<IISerialManager> mIsm;
    AutoPtr<ISerialPort> mSp;
};

} // namespace SerialManagerDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
